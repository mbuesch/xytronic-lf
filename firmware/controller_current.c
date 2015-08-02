/*
 * Xytronic LF-1600
 * Current controller
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

#include "controller_current.h"
#include "pid.h"
#include "timer.h"


/* Current controller PID parameters */
#define CONTRCURR_PID_KP	1.0
#define CONTRCURR_PID_KI	0.2
#define CONTRCURR_PID_KD	0.05
#define CONTRCURR_LIM		10.0
#define CONTRCURR_PERIOD_MS	100	/* milliseconds */


static struct pid current_pid;
static fixpt_t current_feedback;
static fixpt_t current_controlvar;
static struct timer current_timer;


fixpt_t contrcurr_get_controlvar(void)
{
	return current_controlvar;
}

void contrcurr_set_feedback(fixpt_t r)
{
	current_feedback = r;
}

void contrcurr_work(void)
{
	fixpt_t dt, r, y;

	if (!timer_expired(&current_timer))
		return;
	timer_add(&current_timer, CONTRCURR_PERIOD_MS);

	/* Get the feedback (r) */
	r = current_feedback;

	//TODO scale r

	/* Get delta-t, in seconds */
	dt = float_to_fixpt((float)CONTRCURR_PERIOD_MS / 1000.0f);

	y = pid_run(&current_pid, dt, r);

	//TODO scale y

	/* Set the control variable (y) */
	current_controlvar = y;
}

void contrcurr_init(void)
{
	pid_init(&current_pid,
		 float_to_fixpt(CONTRCURR_PID_KP),
		 float_to_fixpt(CONTRCURR_PID_KI),
		 float_to_fixpt(CONTRCURR_PID_KD),
		 float_to_fixpt(CONTRCURR_LIM));
	timer_arm(&current_timer, 0);
}
