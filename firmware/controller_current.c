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


/* Design note:
 *
 * The current controller is a PID controller in the lower
 * setpoint area. However in the upper area the current is
 * set without feedback evaluation. This is due to our feedback
 * hardware not working correctly for high currents.
 * The cut-off-setpoint where the PID is switched off/on
 * is defined via CONTRCURR_PID_CUTOFF_HI/LO.
 */


/* Current controller PID parameters */
#define CONTRCURR_PID_KP	0.2
#define CONTRCURR_PID_KI	0.5
#define CONTRCURR_PID_KD	0.0
/* PID cut off current. PID is only active below this setpoint. */
#define CONTRCURR_PID_CUTOFF_HI	1.7
#define CONTRCURR_PID_CUTOFF_LO	1.0


struct current_contr_context {
	bool enabled;
	bool pid_disabled;
	bool emergency;
	bool restricted;
	struct pid pid;
	fixpt_t feedback;
	fixpt_t prev_y;
	struct timer timer;

	int24_t old_current_feedback;
	int24_t old_current_control;
};

static struct current_contr_context contrcurr;


static void contrcurr_run(fixpt_t r)
{
	fixpt_t dt, y, w;

	if (!contrcurr.enabled)
		return;
	if (contrcurr.emergency)
		return;

	/* Get delta-t that elapsed since last run, in seconds */
	dt = fixpt_div(int_to_fixpt(timer_ms_since(&contrcurr.timer)),
		       int_to_fixpt(1000));
	timer_set_now(&contrcurr.timer);

	/* Check whether the PID should be enabled or disabled. */
	w = pid_get_setpoint(&contrcurr.pid);
	if (contrcurr.pid_disabled) {
		if (w <= float_to_fixpt(CONTRCURR_PID_CUTOFF_LO))
			contrcurr.pid_disabled = false;
	} else {
		if (w >= float_to_fixpt(CONTRCURR_PID_CUTOFF_HI))
			contrcurr.pid_disabled = true;
	}

	if (contrcurr.pid_disabled) {
		/* We are in the upper setpoint area.
		 * Do not use the real feedback. */
		r = contrcurr.prev_y;
	}
	/* Run the PID controller */
	y = pid_run(&contrcurr.pid, dt, r);

	if (contrcurr.restricted) {
		/* We are in restricted mode.
		 * Limit current to the restricted lower area. */
		if (y > float_to_fixpt(CONTRCURR_RESTRICT_MAXCURR))
			y = float_to_fixpt(CONTRCURR_RESTRICT_MAXCURR);
	}

	debug_report_int24(PSTR("cy"), &contrcurr.old_current_control,
			   (int24_t)y);

	/* Reconfigure the PWM unit to output the
	 * requested heater current (y). */
	contrcurr.prev_y = y;
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
		contrcurr.pid_disabled = false;
		contrcurr.prev_y = int_to_fixpt(0);
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
