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

#include <string.h>


#define MENU_SETTEMP_TIMEOUT	3000


enum menu_state {
	MENU_CURTEMP,		/* Show the current temperature. */
	MENU_SETTEMP,		/* Temperature setpoint. */
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

void menu_update_display(void)
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
		disp[3] = 'C';
		disp[4] = '.';
		disp[5] = '\0';
		break;
	case MENU_SETTEMP:
		temp_fixpt = contrtemp_get_setpoint();
		temp_int = (int16_t)fixpt_to_int(temp_fixpt);
		int_to_ascii_align_right(disp, 2,
					 temp_int,
					 (int16_t)CONTRTEMP_NEGLIM,
					 (int16_t)CONTRTEMP_POSLIM);
		disp[3] = 'S';
		disp[4] = '\0';
		break;
	}

	display_show(disp);
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

	menu_update_display();
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
		if (bstate == BSTATE_POSEDGE) {
			if (button == BUTTON_MINUS)
				start_ramping(RAMP_DOWN, settemp_ramp_handler);
			if (button == BUTTON_PLUS)
				start_ramping(RAMP_UP, settemp_ramp_handler);
			menu.state = MENU_SETTEMP;
			timer_arm(&menu.timeout, MENU_SETTEMP_TIMEOUT);
			menu_update_display();
		}
		break;
	case MENU_SETTEMP:
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
	}
}

/* Periodic work. */
void menu_work(void)
{
	/* Menu timeouts */
	switch (menu.state) {
	case MENU_CURTEMP:
		break;
	case MENU_SETTEMP:
		if (timer_expired(&menu.timeout)) {
			menu.state = MENU_CURTEMP;
			stop_ramping();
			menu_update_display();
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
}

void menu_init(void)
{
	memset(&menu, 0, sizeof(menu));
	menu.state = MENU_CURTEMP;

	buttons_register_handler(BUTTON_SET, menu_button_handler);
	buttons_register_handler(BUTTON_PLUS, menu_button_handler);
	buttons_register_handler(BUTTON_MINUS, menu_button_handler);

	menu_update_display();
}
