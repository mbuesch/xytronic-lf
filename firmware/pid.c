/*
 * PID controller
 *
 * Copyright (c) 2015 Michael Buesch <m@bues.ch>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include "pid.h"
#include "util.h"

#include <string.h>


#if PIDVAL_SIZE == 8
typedef int16_t pidval_dbl_t;
#elif PIDVAL_SIZE == 16
typedef int32_t pidval_dbl_t;
#elif PIDVAL_SIZE == 32
typedef int64_t pidval_dbl_t;
#else
# error "Invalid PIDVAL_SIZE"
#endif

#define PIDVAL_MAX	((pidval_t)((1ULL << (PIDVAL_SIZE - 1U)) - 1U))
#define PIDVAL_MIN	((pidval_t)(-PIDVAL_MAX))


static inline pidval_t pidval_add(pidval_t a, pidval_t b)
{
	pidval_dbl_t tmp;

	tmp = (pidval_dbl_t)a + (pidval_dbl_t)b;

	if (tmp >= PIDVAL_MAX)
		return PIDVAL_MAX;
	if (tmp <= PIDVAL_MIN)
		return PIDVAL_MIN;

	return (pidval_t)tmp;
}

static inline pidval_t pidval_sub(pidval_t a, pidval_t b)
{
	pidval_dbl_t tmp;

	tmp = (pidval_dbl_t)a - (pidval_dbl_t)b;

	if (tmp >= PIDVAL_MAX)
		return PIDVAL_MAX;
	if (tmp <= PIDVAL_MIN)
		return PIDVAL_MIN;

	return (pidval_t)tmp;
}

static inline pidval_t pidval_mul(pidval_t a, pidval_t b)
{
	pidval_dbl_t tmp;

	/* Multiply */
	tmp = (pidval_dbl_t)a * (pidval_dbl_t)b;
	/* Round */
	tmp += 1L << (PIDVAL_SHIFT - 1);
	/* Scale */
	tmp >>= PIDVAL_SHIFT;

	if (tmp >= PIDVAL_MAX)
		return PIDVAL_MAX;
	if (tmp <= PIDVAL_MIN)
		return PIDVAL_MIN;

	return (pidval_t)tmp;
}

static inline pidval_t pidval_neg(pidval_t v)
{
	if (v <= PIDVAL_MIN)
		return PIDVAL_MAX;

	return -v;
}

static inline pidval_t pidval_div(pidval_t a, pidval_t b)
{
	pidval_dbl_t tmp;

	/* Scale */
	tmp = (pidval_dbl_t)a << PIDVAL_SHIFT;
	/* Round */
	if (tmp >= 0)
		tmp += b / 2;
	else
		tmp -= b / 2;
	/* Divide */
	tmp /= b;

	return (pidval_t)tmp;
}

pidval_t pid_run(struct pid *pid, pidval_t dt, pidval_t r)
{
	pidval_t e, de;
	pidval_t p, i, d, pid_result;
	pidval_t y_lim_pos = pid->y_lim;
	pidval_t y_lim_neg = pidval_neg(y_lim_pos);

	/* Calculate the deviation. */
	e = pidval_sub(pid->setpoint, r);

	/* P term */
	p = pidval_mul(pid->kp, e);

	/* I term */
	i = pidval_add(pid->integr, pidval_mul(pidval_mul(pid->ki, e), dt));
	i = clamp(i, y_lim_neg, y_lim_pos);
	pid->integr = i;

	/* D term */
	de = pidval_sub(e, pid->prev_e);
	if (dt) {
		d = pidval_div(pidval_mul(de, pid->kd), dt);
	} else {
		if (de < 0)
			d = y_lim_neg;
		else
			d = y_lim_pos;
	}
	pid->prev_e = e;

	/* Add P, I and D terms */
	pid_result = pidval_add(pidval_add(p, i), d);
	pid_result = clamp(pid_result, y_lim_neg, y_lim_pos);

	return pid_result;
}

void pid_init(struct pid *pid,
	      pidval_t kp, pidval_t ki, pidval_t kd,
	      pidval_t y_lim)
{
	memset(pid, 0, sizeof(*pid));
	pid->kp = kp;
	pid->ki = ki;
	pid->kd = kd;
	pid->y_lim = y_lim;
}
