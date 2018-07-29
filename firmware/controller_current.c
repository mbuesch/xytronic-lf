/*
 * Xytronic LF-1600
 * Current controller
 *
 * Copyright (c) 2015-2017 Michael Buesch <m@bues.ch>
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
#include "filter.h"
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
 *
 * CONTRCURR_PID_CUTOFF_HI is configured via CONF_CURRCUTOFF.
 */


enum current_r_state {
	RSTATE_DISABLED,
	RSTATE_SYNCING,
	RSTATE_ENABLED,
};

struct current_contr_context {
	bool enabled;
	bool restricted;
	uint8_t emergency_flags;

	struct pid pid;
	enum current_r_state r_state;
	fixpt_t feedback;
	fixpt_t prev_y;

	struct timer timer;

	struct lp_filter_fixpt model;

	/* Debug state */
	int24_t old_current_real_feedback;
	int24_t old_current_used_feedback;
	int24_t old_current_control;
	int8_t old_r_state;
};

static struct current_contr_context contrcurr;

static const struct pid_k_set __flash contrcurr_factors = {
	.kp		= FLOAT_TO_FIXPT(CONTRCURR_PID_KP),
	.ki		= FLOAT_TO_FIXPT(CONTRCURR_PID_KI),
	.kd		= FLOAT_TO_FIXPT(CONTRCURR_PID_KD),
	.d_decay_div	= FLOAT_TO_FIXPT(CONTRCURR_PID_D_DECAY),
};


static void contrcurr_run(fixpt_t real_r)
{
	fixpt_t dt, y, r;

	if (!contrcurr.enabled)
		return;
	if (contrcurr.emergency_flags)
		return;

	/* Get delta-t that elapsed since last run, in seconds */
	dt = fixpt_div(int_to_fixpt(timer_ms_since(&contrcurr.timer)),
		       int_to_fixpt(1000));
	timer_set_now(&contrcurr.timer);

	r = real_r;

	/* Check whether the actual r or the model shall be used. */
	switch (contrcurr.r_state) {
	case RSTATE_DISABLED:
		/* Usage of real_r is disabled. Use the model r.
		 * Once real_r drops below LO state start an r resync.
		 */
		if (real_r <= float_to_fixpt(CONTRCURR_PID_CUTOFF_LO)) {
			contrcurr.r_state = RSTATE_SYNCING;
		} else {
			/* We are in the upper setpoint area.
			 * Do not use the real feedback. Use a PT1 model instead. */
			r = lp_filter_fixpt_run(&contrcurr.model, contrcurr.prev_y,
						int_to_fixpt(16));
			break;
		}
		/* fall through... */
	case RSTATE_SYNCING:
		/* Slowly re-synchronize the model r to the real_r.
		 * Once we are reasomably close, switch back to 'enabled' state.
		 */
		r = lp_filter_fixpt_run(&contrcurr.model, real_r,
					int_to_fixpt(24));
		if (fixpt_abs(fixpt_sub(r, real_r)) < float_to_fixpt(AMPERE(0.1)))
			contrcurr.r_state = RSTATE_ENABLED;
		break;
	case RSTATE_ENABLED:
	default:
		/* Use real_r unless it rises above HI.
		 */
		if (real_r >= float_to_fixpt(CONTRCURR_PID_CUTOFF_HI)) {
			contrcurr.r_state = RSTATE_DISABLED;
			lp_filter_fixpt_set(&contrcurr.model, r);
		}
		break;
	}

	debug_report_int8(DEBUG_PFX1("rs"), &contrcurr.old_r_state,
			  (int8_t)contrcurr.r_state);
	debug_report_fixpt(DEBUG_PFX1("cr2"),
			   &contrcurr.old_current_used_feedback, r);

	/* Run the PID controller */
	y = pid_run(&contrcurr.pid, dt, r);

	if (contrcurr.restricted) {
		/* We are in restricted mode.
		 * Limit current to the restricted lower area. */
		if (y > float_to_fixpt(CONTRCURR_RESTRICT_MAXCURR))
			y = float_to_fixpt(CONTRCURR_RESTRICT_MAXCURR);
	}

	debug_report_fixpt(DEBUG_PFX1("cy"),
			   &contrcurr.old_current_control, y);

	/* Reconfigure the PWM unit to output the
	 * requested heater current (y). */
	contrcurr.prev_y = y;
	pwmcurr_set(y);
}

void contrcurr_set_feedback(fixpt_t r)
{
	if (r != contrcurr.feedback) {
		contrcurr.feedback = r;
		debug_report_fixpt(DEBUG_PFX1("cr1"),
				   &contrcurr.old_current_real_feedback, r);
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
		contrcurr.r_state = RSTATE_ENABLED;
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

void contrcurr_set_emerg(uint8_t emergency_flags)
{
	if (emergency_flags != contrcurr.emergency_flags) {
		contrcurr.emergency_flags = emergency_flags;
		if (emergency_flags) {
			/* In an emergency situation, disable the
			 * heater current to avoid damage.
			 */
			pwmcurr_set(float_to_fixpt(CONTRCURR_NEGLIM));
		}
	}
}

uint8_t contrcurr_get_emerg(void)
{
	return contrcurr.emergency_flags;
}

void contrcurr_init(void)
{
	struct pid_k_set k_set;

	k_set = contrcurr_factors;
	pid_init(&contrcurr.pid,
#if CONF_DEBUG
		 PSTR("pid-c"),
#endif
		 &k_set,
		 float_to_fixpt(CONTRCURR_NEGLIM_I),
		 float_to_fixpt(CONTRCURR_POSLIM_I),
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
