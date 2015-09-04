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

#include <string.h>


#define MENU_SETTEMP_TIMEOUT	3000


enum menu_state {
	MENU_CURTEMP,		/* Show the current temperature. */
	MENU_SETTEMP,		/* Temperature setpoint. */
	MENU_DEBUG,		/* Debug enable. */
	MENU_MANUAL,		/* Manual mode */
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

	struct timer delay_timer;
	bool delay_running;

	enum ramp_state ramp;
	struct timer ramp_timer;
	ramp_handler_t ramp_handler;
	int32_t ramp_period;

	uint8_t manmode_percentage;

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

	disp[0] = '\0';

	switch (menu.state) {
	case MENU_CURTEMP:
		temp_fixpt = contrtemp_get_feedback();
		temp_int = (int16_t)fixpt_to_int(temp_fixpt);
		int_to_ascii_align_right(disp, 2,
					 temp_int,
					 (int16_t)CONTRTEMP_NEGLIM,
					 (int16_t)CONTRTEMP_POSLIM);
		strcpy_P(disp + 3, PSTR("C."));
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
	case MENU_DEBUG:
		strcpy_P(disp, PSTR("DBG"));
		break;
	case MENU_MANUAL:
		int_to_ascii_align_right(disp, 2,
					 menu.manmode_percentage,
					 0, 100);
		strcpy_P(disp + 3, PSTR("P"));
		break;
	}

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
	switch (menu.state) {
	case MENU_CURTEMP:
		menu.delay_running = false;
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
				menu_set_state(MENU_DEBUG);
				break;
			}
		}
		break;
	case MENU_SETTEMP:
		menu.delay_running = false;
		timer_arm(&menu.timeout, MENU_SETTEMP_TIMEOUT);
		if (bstate == BSTATE_POSEDGE) {
			if (button == BUTTON_MINUS)
				start_ramping(RAMP_DOWN, settemp_ramp_handler);
			if (button == BUTTON_PLUS)
				start_ramping(RAMP_UP, settemp_ramp_handler);
		}
		if (bstate == BSTATE_NEGEDGE)
			stop_ramping();
		break;
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
			if (button == BUTTON_SET) {
				timer_arm(&menu.delay_timer, 1000);
				menu.delay_running = true;
				break;
			}
		}
		if (bstate == BSTATE_NEGEDGE) {
			if (button == BUTTON_SET) {
				if (!debug_is_enabled()) {
					menu_set_state(MENU_CURTEMP);
					break;
				}
			}
		}
		if (bstate == BSTATE_PRESSED) {
			if (button == BUTTON_SET) {
				if (!debug_is_enabled() &&
				    menu.delay_running &&
				    timer_expired(&menu.delay_timer)) {
					menu.manmode_percentage = 0;
					contrcurr_set_enabled(false, menu.manmode_percentage);
					contrtemp_set_enabled(false);
					menu_set_state(MENU_MANUAL);
					break;
				}
			}
		}
		break;
	case MENU_MANUAL:
		if (menu.delay_running) {
			if (bstate == BSTATE_NEGEDGE) {
				menu.delay_running = false;
				break;
			}
			break;
		}
		if (bstate == BSTATE_POSEDGE) {
			if (button == BUTTON_PLUS) {
				if (menu.manmode_percentage <= 90) {
					menu.manmode_percentage = (uint8_t)(menu.manmode_percentage + 10);
					contrcurr_set_enabled(false, menu.manmode_percentage);
					menu_request_display_update();
				}
				break;
			}
			if (button == BUTTON_MINUS) {
				if (menu.manmode_percentage >= 10) {
					menu.manmode_percentage = (uint8_t)(menu.manmode_percentage - 10);
					contrcurr_set_enabled(false, menu.manmode_percentage);
					menu_request_display_update();
				}
				break;
			}
		}
		if (bstate == BSTATE_NEGEDGE) {
			if (button == BUTTON_SET) {
				contrcurr_set_enabled(true, 0);
				contrtemp_set_enabled(true);
				menu_set_state(MENU_CURTEMP);
				break;
			}
		}
		break;
	}
}

/* Periodic work. */
void menu_work(void)
{
	/* Menu timeouts */
	switch (menu.state) {
	case MENU_CURTEMP:
	case MENU_DEBUG:
	case MENU_MANUAL:
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

	if (button_is_pressed(BUTTON_SET)) {
		debug_enable(true);
		menu_set_state(MENU_DEBUG);
	} else {
		menu_set_state(MENU_CURTEMP);
	}
}
