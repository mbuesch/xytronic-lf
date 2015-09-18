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


void pid_reset(struct pid *pid)
{
	pid->prev_e = int_to_fixpt(0);
	pid->integr = int_to_fixpt(0);
}

void pid_set_factors(struct pid *pid,
		     fixpt_t kp, fixpt_t ki, fixpt_t kd)
{
	pid->kp = kp;
	pid->ki = ki;
	pid->kd = kd;
	pid_reset(pid);
}

fixpt_t pid_run(struct pid *pid, fixpt_t dt, fixpt_t r)
{
	fixpt_t e, de;
	fixpt_t kp, ki, kd;
	fixpt_t p, i, d;
	fixpt_t pid_result = int_to_fixpt(0);
	fixpt_t y_neglim = pid->y_neglim;
	fixpt_t y_poslim = pid->y_poslim;

	/* Calculate the deviation. */
	e = fixpt_sub(pid->setpoint, r);

	/* P term */
	kp = pid->kp;
	if (kp != int_to_fixpt(0)) {
		p = fixpt_mul(kp, e);

		pid_result = fixpt_add(pid_result, p);
	}

	/* I term */
	ki = pid->ki;
	if (ki != int_to_fixpt(0)) {
		i = fixpt_add(pid->integr, fixpt_mul(fixpt_mul(ki, e), dt));
		i = clamp(i, y_neglim, y_poslim);
		pid->integr = i;

		pid_result = fixpt_add(pid_result, i);
	}

	/* D term */
	kd = pid->kd;
	if (kd != int_to_fixpt(0)) {
		de = fixpt_sub(e, pid->prev_e);
		if (dt) {
			d = fixpt_mul_div(de, kd, dt);
		} else {
			if (de < 0)
				d = y_neglim;
			else
				d = y_poslim;
		}
		pid->prev_e = fixpt_div(e, pid->d_decay_div);

		pid_result = fixpt_add(pid_result, d);
	}

	pid_result = clamp(pid_result, y_neglim, y_poslim);

	return pid_result;
}

void pid_init(struct pid *pid,
	      fixpt_t kp, fixpt_t ki, fixpt_t kd,
	      fixpt_t y_neglim, fixpt_t y_poslim)
{
	memset(pid, 0, sizeof(*pid));
	pid->y_neglim = y_neglim;
	pid->y_poslim = y_poslim;
	pid_set_factors(pid, kp, ki, kd);
	pid->d_decay_div = int_to_fixpt(1);
}
