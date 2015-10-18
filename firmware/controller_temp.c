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
#include "buttons.h"
#include "debug_uart.h"

#include <string.h>


/* Temperature controller PID parameters */
#define CONTRTEMP_PID_KP_NORMAL		4.0
#define CONTRTEMP_PID_KI_NORMAL		0.08
#define CONTRTEMP_PID_KD_NORMAL		1.0
#define CONTRTEMP_PID_D_DECAY_NORMAL	1.2
/* Temperature controller boost (1) PID parameters */
#define CONTRTEMP_PID_KP_BOOST1		6.0
#define CONTRTEMP_PID_KI_BOOST1		0.08
#define CONTRTEMP_PID_KD_BOOST1		1.0
#define CONTRTEMP_PID_D_DECAY_BOOST1	1.5
/* Temperature controller boost (2) PID parameters */
#define CONTRTEMP_PID_KP_BOOST2		12.0
#define CONTRTEMP_PID_KI_BOOST2		0.1
#define CONTRTEMP_PID_KD_BOOST2		1.5
#define CONTRTEMP_PID_D_DECAY_BOOST2	1.5


struct temp_contr_context {
	bool enabled;
	bool emergency;

	enum contrtemp_boostmode boost_mode;

	struct pid pid;
	fixpt_t feedback;
	struct timer dt_timer;

	int24_t old_temp_feedback;
	int24_t old_temp_control1;
	int24_t old_temp_control2;
};

static struct temp_contr_context contrtemp;

static const struct pid_k_set __flash contrtemp_normal_factors = {
	.kp		= FLOAT_TO_FIXPT(CONTRTEMP_PID_KP_NORMAL),
	.ki		= FLOAT_TO_FIXPT(CONTRTEMP_PID_KI_NORMAL),
	.kd		= FLOAT_TO_FIXPT(CONTRTEMP_PID_KD_NORMAL),
	.d_decay_div	= FLOAT_TO_FIXPT(CONTRTEMP_PID_D_DECAY_NORMAL),
};

static const struct pid_k_set __flash contrtemp_boost1_factors = {
	.kp		= FLOAT_TO_FIXPT(CONTRTEMP_PID_KP_BOOST1),
	.ki		= FLOAT_TO_FIXPT(CONTRTEMP_PID_KI_BOOST1),
	.kd		= FLOAT_TO_FIXPT(CONTRTEMP_PID_KD_BOOST1),
	.d_decay_div	= FLOAT_TO_FIXPT(CONTRTEMP_PID_D_DECAY_BOOST1),
};

static const struct pid_k_set __flash contrtemp_boost2_factors = {
	.kp		= FLOAT_TO_FIXPT(CONTRTEMP_PID_KP_BOOST2),
	.ki		= FLOAT_TO_FIXPT(CONTRTEMP_PID_KI_BOOST2),
	.kd		= FLOAT_TO_FIXPT(CONTRTEMP_PID_KD_BOOST2),
	.d_decay_div	= FLOAT_TO_FIXPT(CONTRTEMP_PID_D_DECAY_BOOST2),
};


void contrtemp_get_pid_factors(enum contrtemp_boostmode for_mode)
{
	//TODO
}

enum contrtemp_boostmode contrtemp_get_boost_mode(void)
{
	return contrtemp.boost_mode;
}

static void contrtemp_set_boost_mode(enum contrtemp_boostmode new_boost_mode)
{
	struct pid_k_set k_set;

	switch (new_boost_mode) {
	case TEMPBOOST_NORMAL:
		k_set = contrtemp_normal_factors;
		break;
	case TEMPBOOST_BOOST1:
		k_set = contrtemp_boost1_factors;
		break;
	case TEMPBOOST_BOOST2:
		k_set = contrtemp_boost2_factors;
		break;
	case NR_BOOST_MODES:
	default:
		return;
	}

	pid_set_factors(&contrtemp.pid, &k_set);
	contrtemp.boost_mode = new_boost_mode;

	debug_print_int16(PSTR("tb"), (int16_t)new_boost_mode);

	menu_request_display_update();
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

static void contrtemp_run(fixpt_t r)
{
	fixpt_t dt, y, y_current;
	uint8_t emergency_flags;

	if (!contrtemp.enabled)
		return;
	if (contrtemp.emergency)
		return;

	/* Get delta-t that elapsed since last run, in seconds */
	dt = fixpt_div(int_to_fixpt(timer_ms_since(&contrtemp.dt_timer)),
		       int_to_fixpt(1000));
	timer_set_now(&contrtemp.dt_timer);

	/* Run the PID controller */
	y = pid_run(&contrtemp.pid, dt, r);

	debug_report_int24(PSTR("ty1"), &contrtemp.old_temp_control1,
			   (int24_t)y);

	/* Map the requested temperature to a heater current. */
	y_current = temp_to_amps(y);

	emergency_flags = contrcurr_get_emerg();
	if (r > float_to_fixpt(CONTRTEMP_POSLIM)) {
		/* The measured temperature is higher than the maximum.
		 * We need to avoid damage.
		 * Disable current by requesting an emergency in
		 * the current controller.
		 */
		emergency_flags |= CONTRCURR_EMERG_HIGH_TEMP;
		y_current = float_to_fixpt(CONTRCURR_NEGLIM);
	} else {
		emergency_flags &= (uint8_t)~CONTRCURR_EMERG_HIGH_TEMP;
	}
	contrcurr_set_emerg(emergency_flags);

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

static void do_set_enabled(bool enabled)
{
	contrtemp.enabled = enabled;

	/* Reset the controller. */
	pid_reset(&contrtemp.pid);
	timer_set_now(&contrtemp.dt_timer);
	contrtemp_set_boost_mode(TEMPBOOST_NORMAL);
}

void contrtemp_set_enabled(bool enabled)
{
	if (contrtemp.enabled != enabled)
		do_set_enabled(enabled);
}

static void do_set_emerg(bool emergency)
{
	contrtemp.emergency = emergency;
	if (emergency) {
		/* In an emergency situation, disable the
		 * heater current to avoid damage.
		 */
		contrcurr_set_setpoint(float_to_fixpt(CONTRCURR_NEGLIM));
	}
}

void contrtemp_set_emerg(bool emergency)
{
	if (emergency != contrtemp.emergency)
		do_set_emerg(emergency);
}

bool contrtemp_in_emerg(void)
{
	return contrtemp.emergency;
}

bool contrtemp_is_heating_up(void)
{
	return contrtemp.feedback < contrtemp_get_setpoint();
}

static void contrtemp_iron_button_handler(enum button_id button,
					  enum button_state bstate)
{
	enum contrtemp_boostmode new_boost_mode;

	if (bstate == BSTATE_POSEDGE) {
		new_boost_mode = contrtemp.boost_mode;
		new_boost_mode++;
		if (new_boost_mode >= NR_BOOST_MODES)
			new_boost_mode = 0;

		contrtemp_set_boost_mode(new_boost_mode);
	}
}

void contrtemp_init(void)
{
	struct pid_k_set k_set;
	struct settings *settings;

	memset(&contrtemp, 0, sizeof(contrtemp));

	k_set = contrtemp_normal_factors;
	pid_init(&contrtemp.pid, &k_set,
		 float_to_fixpt(CONTRTEMP_NEGLIM),
		 float_to_fixpt(CONTRTEMP_POSLIM));

	/* Get the initial setpoint from EEPROM. */
	settings = get_settings();
	pid_set_setpoint(&contrtemp.pid, settings->temp_setpoint);

	/* Enable the controller. */
	do_set_enabled(true);
	do_set_emerg(false);

	/* Register handler for the iron button. */
	buttons_register_handler(BUTTON_IRON,
				 contrtemp_iron_button_handler);
}
