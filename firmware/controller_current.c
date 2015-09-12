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
#include "pwm_current.h"
#include "scale.h"
#include "debug_uart.h"

#include <string.h>


/* Current controller PID parameters */
#define CONTRCURR_PID_KP	0.5
#define CONTRCURR_PID_KI	0.2
#define CONTRCURR_PID_KD	0.0


struct current_contr_context {
	bool enabled;
	bool emergency;
	bool restricted;
	struct pid pid;
	fixpt_t feedback;
	struct timer timer;

	int24_t old_current_feedback;
	int24_t old_current_control;
};

static struct current_contr_context contrcurr;


static void contrcurr_run(fixpt_t r)
{
	fixpt_t dt, y;

	if (!contrcurr.enabled)
		return;
	if (contrcurr.emergency)
		return;

	/* Get delta-t that elapsed since last run, in seconds */
	dt = fixpt_div(int_to_fixpt(timer_ms_since(&contrcurr.timer)),
		       int_to_fixpt(1000));
	timer_set_now(&contrcurr.timer);

	/* Run the PID controller */
	y = pid_run(&contrcurr.pid, dt, r);

	if (contrcurr.restricted) {
		if (y > float_to_fixpt(CONTRCURR_RESTRICT_MAXCURR))
			y = float_to_fixpt(CONTRCURR_RESTRICT_MAXCURR);
	}

	debug_report_int24(PSTR("cy"), &contrcurr.old_current_control,
			   (int24_t)y);

	/* Reconfigure the PWM unit to output the
	 * requested heater current (y). */
	pwmcurr_set(y);
}

void contrcurr_set_feedback(fixpt_t r)
{
	if (r != contrcurr.feedback) {
		contrcurr.feedback = r;
		debug_report_int24(PSTR("cr"),
				   &contrcurr.old_current_feedback,
				   (int24_t)r);
	}

	/* Run the controller. */
	contrcurr_run(r);
}

void contrcurr_set_setpoint(fixpt_t w)
{
	pid_set_setpoint(&contrcurr.pid, w);
}

void contrcurr_set_restricted(bool restricted)
{
	contrcurr.restricted = restricted;
}

void contrcurr_set_enabled(bool enable,
			   uint8_t disabled_curr_percent)
{
	fixpt_t y;

	if (enable != contrcurr.enabled) {
		contrcurr.enabled = enable;

		/* Reset the controller. */
		pwmcurr_set(int_to_fixpt(0));
		pid_reset(&contrcurr.pid);
		timer_set_now(&contrcurr.timer);
	}

	if (!enable) {
		/* The controller is disabled.
		 * Set the disabled-current permanently.
		 */
		y = scale(disabled_curr_percent,
			  0, 100,
			  float_to_fixpt(CONTRCURR_NEGLIM),
			  float_to_fixpt(CONTRCURR_POSLIM));
		pwmcurr_set(y);
	}
}

void contrcurr_set_emerg(bool emergency)
{
	if (emergency != contrcurr.emergency) {
		contrcurr.emergency = emergency;
		if (emergency) {
			/* In an emergency situation, disable the
			 * heater current to avoid damage.
			 */
			pwmcurr_set(float_to_fixpt(CONTRCURR_NEGLIM));
		}
	}
}

bool contrcurr_in_emerg(void)
{
	return contrcurr.emergency;
}

void contrcurr_init(void)
{
	memset(&contrcurr, 0, sizeof(contrcurr));

	pid_init(&contrcurr.pid,
		 float_to_fixpt(CONTRCURR_PID_KP),
		 float_to_fixpt(CONTRCURR_PID_KI),
		 float_to_fixpt(CONTRCURR_PID_KD),
		 float_to_fixpt(CONTRCURR_NEGLIM),
		 float_to_fixpt(CONTRCURR_POSLIM));

	/* Enable the controller. */
	contrcurr_set_enabled(true, 0);
	contrcurr_set_emerg(false);
	/* Assume the iron is cold and start
	 * with restricted current.
	 */
	contrcurr_set_restricted(true);
}
