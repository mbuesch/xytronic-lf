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


/* Current controller PID parameters */
#define CONTRCURR_PID_KP	3.0
#define CONTRCURR_PID_KI	0.05
#define CONTRCURR_PID_KD	0.0
#define CONTRCURR_PERIOD_MS	100	/* milliseconds */


static bool current_contr_enabled;
static bool current_contr_emergency;
static struct pid current_pid;
static fixpt_t current_feedback;
static struct timer current_timer;

static int24_t old_current_feedback;
static int24_t old_current_control;


void contrcurr_set_feedback(fixpt_t r)
{
	current_feedback = r;
}

void contrcurr_set_setpoint(fixpt_t w)
{
	pid_set_setpoint(&current_pid, w);
}

void contrcurr_set_enabled(bool enable,
			   uint8_t disabled_curr_percent)
{
	fixpt_t y;

	if (enable != current_contr_enabled) {
		current_contr_enabled = enable;
		pwmcurr_set(int_to_fixpt(0));
		pid_reset(&current_pid);
		timer_arm(&current_timer, 0);
	}

	if (!enable) {
		y = scale(disabled_curr_percent,
			  0, 100,
			  float_to_fixpt(CONTRCURR_NEGLIM),
			  float_to_fixpt(CONTRCURR_POSLIM));
		pwmcurr_set(y);
	}
}

void contrcurr_set_emerg(bool emergency)
{
	if (emergency != current_contr_emergency) {
		current_contr_emergency = emergency;
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
	return current_contr_emergency;
}

void contrcurr_work(void)
{
	fixpt_t dt, r, y;

	if (!current_contr_enabled)
		return;
	if (current_contr_emergency)
		return;
	if (!timer_expired(&current_timer))
		return;
	timer_add(&current_timer, CONTRCURR_PERIOD_MS);

	/* Get the feedback (r) */
	r = current_feedback;
	debug_report_int24(PSTR("cr"), &old_current_feedback, (int24_t)r);

	/* Get delta-t that elapsed since last run, in seconds */
	dt = float_to_fixpt((float)CONTRCURR_PERIOD_MS / 1000.0f);

	/* Run the PID controller */
	y = pid_run(&current_pid, dt, r);

	debug_report_int24(PSTR("cy"), &old_current_control, (int16_t)y);

	/* Reconfigure the PWM unit to output the
	 * requested heater current (y). */
	pwmcurr_set(y);
}

void contrcurr_init(void)
{
	pid_init(&current_pid,
		 float_to_fixpt(CONTRCURR_PID_KP),
		 float_to_fixpt(CONTRCURR_PID_KI),
		 float_to_fixpt(CONTRCURR_PID_KD),
		 float_to_fixpt(CONTRCURR_NEGLIM),
		 float_to_fixpt(CONTRCURR_POSLIM));

	contrcurr_set_enabled(true, 0);
	contrcurr_set_emerg(false);
}
