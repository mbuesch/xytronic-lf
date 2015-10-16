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

#include <string.h>


#define MENU_SETTEMP_TIMEOUT	3000


enum menu_state {
	MENU_CURTEMP,		/* Show the current temperature. */
	MENU_SETTEMP,		/* Temperature setpoint. */
#ifdef CONF_DEBUG
	MENU_DEBUG,		/* Debug enable. */
#endif
#ifdef CONF_CALIB
	MENU_CALIB,		/* Current calibration. */
#endif
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
				     int16_t min_val, int16_t max_val)
{
	uint8_t i;

	val = clamp(val, min_val, max_val);
	memset(buf, 0, align_to_digit + 1u);
	itoa(val, buf, 10);
	if (align_to_digit > 0) {
		while (buf[align_to_digit] == '\0') {
			for (i = align_to_digit; i > 0; i--)
				buf[i] = buf[i - 1];
			buf[0] = ' ';
		}
	}
}

static void menu_update_display(void)
{
	char disp[6];
	fixpt_t temp_fixpt;
	int16_t temp_int;
	uint8_t displayed_error;
	bool displayed_heating;

	displayed_error = menu.displayed_error;
	displayed_heating = menu.displayed_heating;
	disp[0] = '\0';

	if (displayed_error) {
		strcpy_P(disp, PSTR("Err"));
		int_to_ascii_align_right(disp + 3, 0,
					 displayed_error, 0, 9);
		disp[5] = '\0';
	} else {
		switch (menu.state) {
		case MENU_CURTEMP:
			temp_fixpt = contrtemp_get_feedback();
			temp_int = (int16_t)fixpt_to_int(temp_fixpt);
			int_to_ascii_align_right(disp, 2,
						 temp_int,
						 (int16_t)CONTRTEMP_NEGLIM,
						 (int16_t)CONTRTEMP_POSLIM);
			switch (contrtemp_get_boost_mode()) {
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
						 (int16_t)CONTRTEMP_POSLIM);
			strcpy_P(disp + 3, PSTR("S"));
			break;
#ifdef CONF_DEBUG
		case MENU_DEBUG:
			strcpy_P(disp, PSTR("DBG"));
			break;
#endif
#ifdef CONF_CALIB
		case MENU_CALIB:
			strcpy_P(disp, PSTR("CAL"));
			break;
#endif
		}
	}

	/* Show the 'is heating' dot. */
	display_force_dp(2, true,
			 (displayed_heating && !displayed_error));
	/* Update the display content. */
	display_show(disp);
}

void menu_request_display_update(void)
{
	menu.display_update_requested = 1;
	mb();
}

static void menu_set_state(enum menu_state new_state)
{
	menu.state = new_state;
	menu_request_display_update();
}

static void settemp_ramp_handler(bool up)
{
	fixpt_t setpoint;

	setpoint = contrtemp_get_setpoint();
	if (up) {
		setpoint = fixpt_add(setpoint, float_to_fixpt(1.0));
	} else {
		setpoint = fixpt_sub(setpoint, float_to_fixpt(1.0));
	}
	if (setpoint > float_to_fixpt(CONTRTEMP_POSLIM))
		setpoint = float_to_fixpt(CONTRTEMP_POSLIM);
	if (setpoint < float_to_fixpt(CONTRTEMP_NEGLIM))
		setpoint = float_to_fixpt(CONTRTEMP_NEGLIM);
	contrtemp_set_setpoint(setpoint);

	menu_request_display_update();
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
				timer_arm(&menu.timeout, MENU_SETTEMP_TIMEOUT);
				break;
			}
		}
		if (bstate == BSTATE_NEGEDGE) {
			if (button == BUTTON_SET) {
#ifdef CONF_DEBUG
				menu_set_state(MENU_DEBUG);
#endif
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
				menu_set_state(MENU_CURTEMP);
				break;
			}
		}
		break;
#ifdef CONF_DEBUG
	case MENU_DEBUG:
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
#ifdef CONF_CALIB
					menu_set_state(MENU_CALIB);
#else /* CONF_CALIB */
					menu_set_state(MENU_CURTEMP);
#endif /* CONF_CALIB */
					break;
				}
			}
		}
		break;
#endif /* CONF_DEBUG */
#ifdef CONF_CALIB
	case MENU_CALIB:
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
				menu_set_state(MENU_CURTEMP);
				break;
			}
		}
		break;
#endif /* CONF_CALIB */
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
#ifdef CONF_DEBUG
	case MENU_DEBUG:
#endif
#ifdef CONF_CALIB
	case MENU_CALIB:
#endif
		break;
	case MENU_SETTEMP:
		if (timer_expired(&menu.timeout)) {
			menu_set_state(MENU_CURTEMP);
			stop_ramping();
			break;
		}
		break;
	}

	/* Generic ramping handler. */
	if (menu.ramp != RAMP_NONE) {
		if (timer_expired(&menu.ramp_timer)) {
			menu.ramp_handler(menu.ramp == RAMP_UP);
			timer_add(&menu.ramp_timer, menu.ramp_period);
			if (menu.ramp_period > RAMP_MIN_PERIOD_MS)
				menu.ramp_period /= 2;
		}
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

#ifdef CONF_DEBUG
	if (button_is_pressed(BUTTON_SET)) {
		debug_enable(true);
		menu_set_state(MENU_DEBUG);
	} else {
		menu_set_state(MENU_CURTEMP);
	}
#else /* CONF_DEBUG */
	menu_set_state(MENU_CURTEMP);
#endif /* CONF_DEBUG */
}
