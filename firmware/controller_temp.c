/*
 * Xytronic LF-1600
 * Temperature controller
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

#include "controller_temp.h"
#include "controller_current.h"
#include "pid.h"
#include "timer.h"
#include "scale.h"
#include "settings.h"
#include "menu.h"
#include "debug_uart.h"

#include <string.h>


/* Temperature controller PID parameters */
#define CONTRTEMP_PID_KP	1.5
#define CONTRTEMP_PID_KI	0.05
#define CONTRTEMP_PID_KD	0.2
#define CONTRTEMP_PERIOD_MS	50	/* milliseconds */


struct temp_contr_context {
	bool enabled;
	bool emergency;
	struct pid pid;
	fixpt_t feedback;
	struct timer timer;

	int24_t old_temp_feedback;
	int24_t old_temp_control1;
	int24_t old_temp_control2;
};

static struct temp_contr_context contrtemp;


static fixpt_t temp_to_amps(fixpt_t temp)
{
	fixpt_t current;

	current = rescale(temp,
			  float_to_fixpt(CONTRTEMP_NEGLIM),
			  float_to_fixpt(CONTRTEMP_POSLIM),
			  float_to_fixpt(CONTRCURR_NEGLIM),
			  float_to_fixpt(CONTRCURR_POSLIM));

	return current;
}

static void contrtemp_run(fixpt_t r)
{
	fixpt_t dt, y, y_current;

	if (!contrtemp.enabled)
		return;
	if (contrtemp.emergency)
		return;

	/* Get delta-t that elapsed since last run, in seconds */
	dt = fixpt_div(int_to_fixpt(timer_ms_since(&contrtemp.timer)),
		       int_to_fixpt(1000));
	timer_set_now(&contrtemp.timer);

	/* Run the PID controller */
	y = pid_run(&contrtemp.pid, dt, r);

	debug_report_int24(PSTR("ty1"), &contrtemp.old_temp_control1,
			   (int24_t)y);

	/* Map the requested temperature to a heater current. */
	y_current = temp_to_amps(y);

	debug_report_int24(PSTR("ty2"), &contrtemp.old_temp_control2,
			   (int24_t)y_current);

	/* Set the current controller setpoint to the requested current. */
	contrcurr_set_setpoint(y_current);
}

void contrtemp_set_feedback(fixpt_t r)
{
	if (r != contrtemp.feedback) {
		contrtemp.feedback = r;
		debug_report_int24(PSTR("tr"),
				   &contrtemp.old_temp_feedback,
				   (int24_t)r);
		menu_request_display_update();
	}

	/* Run the controller. */
	contrtemp_run(r);
}

fixpt_t contrtemp_get_feedback(void)
{
	return contrtemp.feedback;
}

void contrtemp_set_setpoint(fixpt_t w)
{
	struct settings *settings;

	pid_set_setpoint(&contrtemp.pid, w);

	settings = get_settings();
	settings->temp_setpoint = w;
	store_settings();
}

fixpt_t contrtemp_get_setpoint(void)
{
	return pid_get_setpoint(&contrtemp.pid);
}

void contrtemp_set_enabled(bool enabled)
{
	if (contrtemp.enabled != enabled) {
		contrtemp.enabled = enabled;

		/* Reset the controller. */
		pid_reset(&contrtemp.pid);
		timer_set_now(&contrtemp.timer);
	}
}

void contrtemp_set_emerg(bool emergency)
{
	if (emergency != contrtemp.emergency) {
		contrtemp.emergency = emergency;
		if (emergency) {
			/* In an emergency situation, disable the
			 * heater current to avoid damage.
			 */
			contrcurr_set_setpoint(float_to_fixpt(CONTRCURR_NEGLIM));
		}
	}
}

bool contrtemp_in_emerg(void)
{
	return contrtemp.emergency;
}

bool contrtemp_is_heating_up(void)
{
	bool heating;
	fixpt_t diff;

	diff = fixpt_sub(contrtemp_get_setpoint(), contrtemp.feedback);
	heating = (diff > float_to_fixpt(2.0));

	return heating;
}

void contrtemp_init(void)
{
	struct settings *settings;

	memset(&contrtemp, 0, sizeof(contrtemp));

	pid_init(&contrtemp.pid,
		 float_to_fixpt(CONTRTEMP_PID_KP),
		 float_to_fixpt(CONTRTEMP_PID_KI),
		 float_to_fixpt(CONTRTEMP_PID_KD),
		 float_to_fixpt(CONTRTEMP_NEGLIM),
		 float_to_fixpt(CONTRTEMP_POSLIM));

	/* Get the initial setpoint from EEPROM. */
	settings = get_settings();
	pid_set_setpoint(&contrtemp.pid, settings->temp_setpoint);

	/* Enable the controller. */
	contrtemp_set_enabled(true);
	contrtemp_set_emerg(false);
}
