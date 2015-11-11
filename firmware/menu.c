/*
 * Xytronic LF-1600
 * User menu
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

#include "menu.h"
#include "display.h"
#include "buttons.h"
#include "timer.h"
#include "controller_temp.h"
#include "controller_current.h"
#include "debug_uart.h"
#include "calib_current.h"
#include "settings.h"

#include <string.h>


#define MENU_SETTEMP_TIMEOUT	3000
#define MENU_KCONF_PRE_TIMEOUT	1000


enum menu_state {
	MENU_CURTEMP,		/* Show the current temperature. */
	MENU_SETTEMP,		/* Temperature setpoint. */
	MENU_DEBUG,		/* Debug enable. */
	MENU_CALIB,		/* Current calibration. */
	MENU_KP_PRE,		/* Temp KP config */
	MENU_KP,		/* Temp KP config */
	MENU_KI_PRE,		/* Temp KI config */
	MENU_KI,		/* Temp KI config */
	MENU_KD_PRE,		/* Temp KD config */
	MENU_KD,		/* Temp KD config */
};

enum ramp_state {
	RAMP_NONE,
	RAMP_UP,
	RAMP_DOWN,
};

#define RAMP_START_PERIOD_MS	400
#define RAMP_MIN_PERIOD_MS	(RAMP_START_PERIOD_MS >> 4)

typedef void (*ramp_handler_t)(bool up);

struct menu_context {
	enum menu_state state;
	struct timer timeout;

	enum ramp_state ramp;
	struct timer ramp_timer;
	ramp_handler_t ramp_handler;
	int32_t ramp_period;

	uint8_t displayed_error;
	bool displayed_heating;

	uint8_t display_update_requested;
};

static struct menu_context menu;


static void int_to_ascii_align_right(char *buf, uint8_t align_to_digit,
				     int16_t val,
				     int16_t min_val, int16_t max_val,
				     char fill_char)
{
	uint8_t i;

	val = clamp(val, min_val, max_val);
	memset(buf, 0, align_to_digit + 1u);
	itoa(val, buf, 10);
	if (align_to_digit > 0) {
		while (buf[align_to_digit] == '\0') {
			for (i = align_to_digit; i > 0; i--)
				buf[i] = buf[i - 1];
			buf[0] = fill_char;
		}
	}
}

static void menu_update_display(void)
{
	char disp[6];
	fixpt_t temp_fixpt, k;
	int16_t temp_int;
	uint8_t displayed_error;
	bool displayed_heating;
	enum contrtemp_boostmode boost_mode;
	int16_t ipart, fpart;

	boost_mode = contrtemp_get_boost_mode();
	displayed_error = menu.displayed_error;
	displayed_heating = menu.displayed_heating;
	disp[0] = '\0';

	if (displayed_error) {
		strcpy_P(disp, PSTR("Err"));
		int_to_ascii_align_right(disp + 3, 0,
					 displayed_error, 0, 9, ' ');
		disp[5] = '\0';
		displayed_heating = false;
	} else {
		switch (menu.state) {
		case MENU_CURTEMP:
			temp_fixpt = contrtemp_get_feedback();
			temp_int = (int16_t)fixpt_to_int(temp_fixpt);
			int_to_ascii_align_right(disp, 2,
						 temp_int,
						 (int16_t)CONTRTEMP_NEGLIM,
						 (int16_t)CONTRTEMP_POSLIM,
						 ' ');
			switch (boost_mode) {
			case TEMPBOOST_NORMAL:
				strcpy_P(disp + 3, PSTR("C."));
				break;
			case TEMPBOOST_BOOST1:
				strcpy_P(disp + 3, PSTR("b."));
				break;
			case TEMPBOOST_BOOST2:
				strcpy_P(disp + 3, PSTR("8."));
				break;
			case NR_BOOST_MODES:
			default:
				break;
			}
			break;
		case MENU_SETTEMP:
			temp_fixpt = contrtemp_get_setpoint();
			temp_int = (int16_t)fixpt_to_int(temp_fixpt);
			int_to_ascii_align_right(disp, 2,
						 temp_int,
						 (int16_t)CONTRTEMP_NEGLIM,
						 (int16_t)CONTRTEMP_POSLIM,
						 ' ');
			strcpy_P(disp + 3, PSTR("S"));
			break;
		case MENU_DEBUG:
			if (!CONF_DEBUG)
				break;
			strcpy_P(disp, PSTR("DBG"));
			break;
		case MENU_CALIB:
			if (!CONF_CALIB)
				break;
			strcpy_P(disp, PSTR("CAL"));
			break;
		case MENU_KP_PRE:
			if (!CONF_KCONF)
				break;
			strcpy_P(disp, PSTR("CO. P"));
			displayed_heating = false;
			break;
		case MENU_KP:
			if (!CONF_KCONF)
				break;
			k = get_settings()->temp_k[boost_mode].kp;
			ipart = (int16_t)clamp(fixpt_get_int_part(k), 0, 99);
			fpart = (int16_t)fixpt_get_dec_fract(k, 2);
			int_to_ascii_align_right(&disp[0], 1, ipart, 0, 99, ' ');
			disp[2] = '.';
			int_to_ascii_align_right(&disp[3], 1, fpart, 0, 99, '0');
			displayed_heating = false;
			break;
		case MENU_KI_PRE:
			if (!CONF_KCONF)
				break;
			strcpy_P(disp, PSTR("CO. I"));
			displayed_heating = false;
			break;
		case MENU_KI:
			if (!CONF_KCONF)
				break;
			k = get_settings()->temp_k[boost_mode].ki;
			ipart = (int16_t)clamp(fixpt_get_int_part(k), 0, 9);
			fpart = (int16_t)fixpt_get_dec_fract(k, 3);
			int_to_ascii_align_right(&disp[0], 0, ipart, 0, 9, ' ');
			disp[1] = '.';
			int_to_ascii_align_right(&disp[2], 2, fpart, 0, 999, '0');
			displayed_heating = false;
			break;
		case MENU_KD_PRE:
			if (!CONF_KCONF)
				break;
			strcpy_P(disp, PSTR("CO. D"));
			displayed_heating = false;
			break;
		case MENU_KD:
			if (!CONF_KCONF)
				break;
			k = get_settings()->temp_k[boost_mode].kd;
			ipart = (int16_t)clamp(fixpt_get_int_part(k), 0, 99);
			fpart = (int16_t)fixpt_get_dec_fract(k, 2);
			int_to_ascii_align_right(&disp[0], 1, ipart, 0, 99, ' ');
			disp[2] = '.';
			int_to_ascii_align_right(&disp[3], 1, fpart, 0, 99, '0');
			displayed_heating = false;
			break;
		}
	}

	/* Show the 'is heating' dot. */
	display_force_dp(2, true, displayed_heating);
	/* Update the display content. */
	display_show(disp);
}

void menu_request_display_update(void)
{
	menu.display_update_requested = 1;
	mb();
}

static enum menu_state next_menu_state(enum menu_state cur_state)
{
	switch (cur_state) {
	case MENU_CURTEMP:
		if (CONF_DEBUG)
			return MENU_DEBUG;
		if (CONF_CALIB)
			return MENU_CALIB;
		if (CONF_KCONF)
			return MENU_KP_PRE;
		return MENU_CURTEMP;
	case MENU_SETTEMP:
		return MENU_CURTEMP;
	case MENU_DEBUG:
		if (CONF_CALIB)
			return MENU_CALIB;
		if (CONF_KCONF)
			return MENU_KP_PRE;
		return MENU_CURTEMP;
	case MENU_CALIB:
		if (CONF_KCONF)
			return MENU_KP_PRE;
		return MENU_CURTEMP;
	case MENU_KP_PRE:
		return MENU_KP;
	case MENU_KP:
		return MENU_KI_PRE;
	case MENU_KI_PRE:
		return MENU_KI;
	case MENU_KI:
		return MENU_KD_PRE;
	case MENU_KD_PRE:
		return MENU_KD;
	case MENU_KD:
		return MENU_CURTEMP;
	}

	return MENU_CURTEMP;
}

static void menu_set_state(enum menu_state new_state)
{
	menu.state = new_state;

	switch (new_state) {
	case MENU_SETTEMP:
		timer_arm(&menu.timeout, MENU_SETTEMP_TIMEOUT);
		break;
	case MENU_KP_PRE:
	case MENU_KI_PRE:
	case MENU_KD_PRE:
		if (!CONF_KCONF)
			break;
		timer_arm(&menu.timeout, MENU_KCONF_PRE_TIMEOUT);
		/* fall through... */
	case MENU_KP:
	case MENU_KI:
	case MENU_KD:
		if (!CONF_KCONF)
			break;
		contrtemp_set_enabled(false);
		break;
	case MENU_CURTEMP:
		contrtemp_set_enabled(true);
		break;
	case MENU_DEBUG:
	case MENU_CALIB:
		break;
	}

	menu_request_display_update();
}

static void menu_set_next_state(void)
{
	menu_set_state(next_menu_state(menu.state));
}

static void settemp_ramp_handler(bool up)
{
	fixpt_t setpoint;

	setpoint = fixpt_add_limited(contrtemp_get_setpoint(),
				     (up ? float_to_fixpt(1.0) :
					   float_to_fixpt(-1.0)),
				     float_to_fixpt(CONTRTEMP_NEGLIM),
				     float_to_fixpt(CONTRTEMP_POSLIM));
	contrtemp_set_setpoint(setpoint);
}

static void kconf_kp_ramp_handler(bool up)
{
	fixpt_t *k;

	k = &(get_settings()->temp_k[contrtemp_get_boost_mode()].kp);
	*k = fixpt_add_limited(*k, (up ? float_to_fixpt(0.03125) :
					 float_to_fixpt(-0.03125)),
			       float_to_fixpt(0.0),
			       float_to_fixpt(99.0));
	store_settings();
	contrtemp_update_pid_config();
}

static void kconf_ki_ramp_handler(bool up)
{
	fixpt_t *k;

	k = &(get_settings()->temp_k[contrtemp_get_boost_mode()].ki);
	*k = fixpt_add_limited(*k, (up ? float_to_fixpt(0.015625) :
					 float_to_fixpt(-0.015625)),
			       float_to_fixpt(0.0),
			       float_to_fixpt(9.0));
	store_settings();
	contrtemp_update_pid_config();
}

static void kconf_kd_ramp_handler(bool up)
{
	fixpt_t *k;

	k = &(get_settings()->temp_k[contrtemp_get_boost_mode()].kd);
	*k = fixpt_add_limited(*k, (up ? float_to_fixpt(0.03125) :
					 float_to_fixpt(-0.03125)),
			       float_to_fixpt(0.0),
			       float_to_fixpt(99.0));
	store_settings();
	contrtemp_update_pid_config();
}

static void start_ramping(enum ramp_state ramp, ramp_handler_t handler)
{
	menu.ramp = ramp;
	menu.ramp_handler = handler;
	menu.ramp_period = RAMP_START_PERIOD_MS;
	timer_arm(&menu.ramp_timer, 0);
}

static void stop_ramping(void)
{
	menu.ramp = RAMP_NONE;
}

/* Handler for SET, MINUS and PLUS buttons */
static void menu_button_handler(enum button_id button,
				enum button_state bstate)
{
	if (menu.displayed_error) {
		/* We have an error. Ignore all buttons. */
		stop_ramping();
		return;
	}

	switch (menu.state) {
	case MENU_CURTEMP:
		if (bstate == BSTATE_POSEDGE) {
			if (button == BUTTON_MINUS ||
			    button == BUTTON_PLUS) {
				start_ramping((button == BUTTON_MINUS) ?
						RAMP_DOWN : RAMP_UP,
					      settemp_ramp_handler);
				menu_set_state(MENU_SETTEMP);
				break;
			}
		}
		if (bstate == BSTATE_NEGEDGE) {
			if (button == BUTTON_SET) {
				menu_set_next_state();
				break;
			}
		}
		break;
	case MENU_SETTEMP:
		timer_arm(&menu.timeout, MENU_SETTEMP_TIMEOUT);
		if (bstate == BSTATE_POSEDGE) {
			if (button == BUTTON_MINUS) {
				start_ramping(RAMP_DOWN, settemp_ramp_handler);
				break;
			}
			if (button == BUTTON_PLUS) {
				start_ramping(RAMP_UP, settemp_ramp_handler);
				break;
			}
		}
		if (bstate == BSTATE_NEGEDGE) {
			stop_ramping();
			if (button == BUTTON_SET) {
				menu_set_next_state();
				break;
			}
		}
		break;
	case MENU_DEBUG:
		if (!CONF_DEBUG)
			break;
		if (bstate == BSTATE_POSEDGE) {
			if (button == BUTTON_PLUS) {
				debug_enable(true);
				break;
			}
			if (button == BUTTON_MINUS) {
				debug_enable(false);
				menu_request_display_update();
				break;
			}
		}
		if (bstate == BSTATE_NEGEDGE) {
			if (button == BUTTON_SET) {
				if (!debug_is_enabled()) {
					menu_set_next_state();
					break;
				}
			}
		}
		break;
	case MENU_CALIB:
		if (!CONF_CALIB)
			break;
		if (bstate == BSTATE_POSEDGE) {
			if (button == BUTTON_PLUS ||
			    button == BUTTON_MINUS) {
				calcurr_set_enabled(!calcurr_is_enabled());
				menu_request_display_update();
				break;
			}
		}
		if (bstate == BSTATE_NEGEDGE) {
			if (button == BUTTON_SET) {
				calcurr_set_enabled(false);
				menu_set_next_state();
				break;
			}
		}
		break;
	case MENU_KP_PRE:
		break;
	case MENU_KP:
		if (!CONF_KCONF)
			break;
		if (bstate == BSTATE_POSEDGE) {
			if (button == BUTTON_MINUS ||
			    button == BUTTON_PLUS) {
				start_ramping((button == BUTTON_MINUS) ?
						RAMP_DOWN : RAMP_UP,
					      kconf_kp_ramp_handler);
			}
		}
		if (bstate == BSTATE_NEGEDGE) {
			stop_ramping();
			if (button == BUTTON_SET) {
				menu_set_next_state();
				break;
			}
		}
		break;
	case MENU_KI_PRE:
		break;
	case MENU_KI:
		if (!CONF_KCONF)
			break;
		if (bstate == BSTATE_POSEDGE) {
			if (button == BUTTON_MINUS ||
			    button == BUTTON_PLUS) {
				start_ramping((button == BUTTON_MINUS) ?
						RAMP_DOWN : RAMP_UP,
					      kconf_ki_ramp_handler);
			}
		}
		if (bstate == BSTATE_NEGEDGE) {
			stop_ramping();
			if (button == BUTTON_SET) {
				menu_set_next_state();
				break;
			}
		}
		break;
	case MENU_KD_PRE:
		break;
	case MENU_KD:
		if (!CONF_KCONF)
			break;
		if (bstate == BSTATE_POSEDGE) {
			if (button == BUTTON_MINUS ||
			    button == BUTTON_PLUS) {
				start_ramping((button == BUTTON_MINUS) ?
						RAMP_DOWN : RAMP_UP,
					      kconf_kd_ramp_handler);
			}
		}
		if (bstate == BSTATE_NEGEDGE) {
			stop_ramping();
			if (button == BUTTON_SET) {
				menu_set_next_state();
				break;
			}
		}
		break;
	}
}

/* Periodic work. */
void menu_work(void)
{
	bool heating;
	uint8_t error;

	/* Menu timeouts */
	switch (menu.state) {
	case MENU_CURTEMP:
	case MENU_DEBUG:
	case MENU_CALIB:
	case MENU_KP:
	case MENU_KI:
	case MENU_KD:
		break;
	case MENU_KP_PRE:
	case MENU_KI_PRE:
	case MENU_KD_PRE:
	case MENU_SETTEMP:
		if (timer_expired(&menu.timeout)) {
			menu_set_next_state();
			stop_ramping();
			break;
		}
		break;
	}

	/* Evaluate error conditions. */
	error = 0;
	if (contrcurr_get_emerg() & CONTRCURR_EMERG_UNPLAUS_FEEDBACK)
		error |= 1;
	if (contrtemp_in_emerg())
		error |= 2;
	if (error != menu.displayed_error) {
		menu.displayed_error = error;
		menu.display_update_requested = true;
	}

	/* Update heating condition. */
	heating = contrtemp_is_heating_up();
	if (heating != menu.displayed_heating) {
		menu.displayed_heating = heating;
		menu.display_update_requested = true;
	}

	/* Generic ramping handler. */
	if (menu.ramp != RAMP_NONE) {
		if (menu.displayed_error) {
			stop_ramping();
		} else {
			if (timer_expired(&menu.ramp_timer)) {
				menu.ramp_handler(menu.ramp == RAMP_UP);
				menu_request_display_update();
				timer_add(&menu.ramp_timer, menu.ramp_period);
				if (menu.ramp_period > RAMP_MIN_PERIOD_MS)
					menu.ramp_period /= 2;
			}
		}
	}

	/* Update the display, if requested. */
	mb();
	if (menu.display_update_requested) {
		menu.display_update_requested = 0;
		mb();
		menu_update_display();
	}
}

void menu_init(void)
{
	memset(&menu, 0, sizeof(menu));

	buttons_register_handler(BUTTON_SET, menu_button_handler);
	buttons_register_handler(BUTTON_PLUS, menu_button_handler);
	buttons_register_handler(BUTTON_MINUS, menu_button_handler);

	if (CONF_DEBUG) {
		if (button_is_pressed(BUTTON_SET)) {
			debug_enable(true);
			menu_set_state(MENU_DEBUG);
		} else {
			menu_set_state(MENU_CURTEMP);
		}
	} else {
		menu_set_state(MENU_CURTEMP);
	}
}
