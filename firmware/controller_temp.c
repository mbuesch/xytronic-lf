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


/* Temperature controller PID parameters */
#define CONTRTEMP_PID_KP	2.0
#define CONTRTEMP_PID_KI	0.1
#define CONTRTEMP_PID_KD	0.0
#define CONTRTEMP_PERIOD_MS	50	/* milliseconds */


static bool temp_contr_enabled;
static struct pid temp_pid;
static fixpt_t temp_feedback;
static struct timer temp_timer;

static int24_t old_temp_feedback;
static int24_t old_temp_control1;
static int24_t old_temp_control2;


void contrtemp_set_feedback(fixpt_t r)
{
	if (r != temp_feedback) {
		temp_feedback = r;
		menu_update_display();
	}
}

fixpt_t contrtemp_get_feedback(void)
{
	return temp_feedback;
}

void contrtemp_set_setpoint(fixpt_t w)
{
	struct settings *settings;

	pid_set_setpoint(&temp_pid, w);

	settings = get_settings();
	settings->temp_setpoint = w;
	store_settings();
}

fixpt_t contrtemp_get_setpoint(void)
{
	return pid_get_setpoint(&temp_pid);
}

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

void contrtemp_set_enabled(bool enabled)
{
	if (temp_contr_enabled != enabled) {
		temp_contr_enabled = enabled;
		pid_reset(&temp_pid);
		timer_arm(&temp_timer, 0);
	}
}

void contrtemp_work(void)
{
	fixpt_t dt, r, y, y_current;

	if (!temp_contr_enabled)
		return;
	if (!timer_expired(&temp_timer))
		return;
	timer_add(&temp_timer, CONTRTEMP_PERIOD_MS);

	/* Get the feedback (r) */
	r = temp_feedback;
	debug_report_int24(PSTR("tr"), &old_temp_feedback, (int24_t)r);

	/* Get delta-t that elapsed since last run, in seconds */
	dt = float_to_fixpt((float)CONTRTEMP_PERIOD_MS / 1000.0f);

	/* Run the PID controller */
	y = pid_run(&temp_pid, dt, r);

	debug_report_int24(PSTR("ty1"), &old_temp_control1, (int16_t)y);

	/* Map the requested temperature to a heater current. */
	y_current = temp_to_amps(y);

	debug_report_int24(PSTR("ty2"), &old_temp_control2, (int16_t)y_current);

	/* Set the current controller setpoint to the requested current. */
	contrcurr_set_setpoint(y_current);
}

void contrtemp_init(void)
{
	struct settings *settings;

	pid_init(&temp_pid,
		 float_to_fixpt(CONTRTEMP_PID_KP),
		 float_to_fixpt(CONTRTEMP_PID_KI),
		 float_to_fixpt(CONTRTEMP_PID_KD),
		 float_to_fixpt(CONTRTEMP_NEGLIM),
		 float_to_fixpt(CONTRTEMP_POSLIM));

	settings = get_settings();
	pid_set_setpoint(&temp_pid, settings->temp_setpoint);

	contrtemp_set_enabled(true);
}
