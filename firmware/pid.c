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


fixpt_t pid_run(struct pid *pid, fixpt_t dt, fixpt_t r)
{
	fixpt_t e, de;
	fixpt_t p, i, d, pid_result;
	fixpt_t y_lim_pos = pid->y_lim;
	fixpt_t y_lim_neg = fixpt_neg(y_lim_pos);

	/* Calculate the deviation. */
	e = fixpt_sub(pid->setpoint, r);

	/* P term */
	p = fixpt_mul(pid->kp, e);

	/* I term */
	i = fixpt_add(pid->integr, fixpt_mul(fixpt_mul(pid->ki, e), dt));
	i = clamp(i, y_lim_neg, y_lim_pos);
	pid->integr = i;

	/* D term */
	de = fixpt_sub(e, pid->prev_e);
	if (dt) {
		d = fixpt_mul_div(de, pid->kd, dt);
	} else {
		if (de < 0)
			d = y_lim_neg;
		else
			d = y_lim_pos;
	}
	pid->prev_e = e;

	/* Add P, I and D terms */
	pid_result = fixpt_add(fixpt_add(p, i), d);
	pid_result = clamp(pid_result, y_lim_neg, y_lim_pos);

	return pid_result;
}

void pid_init(struct pid *pid,
	      fixpt_t kp, fixpt_t ki, fixpt_t kd,
	      fixpt_t y_lim)
{
	memset(pid, 0, sizeof(*pid));
	pid->kp = kp;
	pid->ki = ki;
	pid->kd = kd;
	pid->y_lim = y_lim;
}
