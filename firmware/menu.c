/*
 * Xytronic LF-1600
 * User menu
 *
 * Copyright (c) 2015-2016 Michael Buesch <m@bues.ch>
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
#include "presets.h"

#include <string.h>


#define MENU_SELPRESET_TIMEOUT	3000
#define MENU_CHGPRESET_TIMEOUT	3000
#define MENU_IDLETEMP_TIMEOUT	3000
#define MENU_KCONF_PRE_TIMEOUT	1500


enum menu_state {
	MENU_CURTEMP,		/* Show the current temperature. */
	MENU_SELPRESET,		/* Select temperature preset. */
	MENU_CHGPRESET,		/* Change temperature preset. */
	MENU_IDLETEMP,		/* Idle temperature setpoint. */
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
	RAMP_PRE_UP,
	RAMP_PRE_DOWN,
};

#define RAMP_PRE_TIMEOUT_MS	1000u
#define RAMP_START_PERIOD_MS	400u
#define RAMP_MIN_PERIOD_MS	(RAMP_START_PERIOD_MS >> 4)

typedef void (*ramp_handler_t)(bool up);

struct menu_context {
	enum menu_state state;
	struct timer timeout;

	enum ramp_state ramp;
	struct timer ramp_timer;
	struct timer ramp_pre_timer;
	ramp_handler_t ramp_handler;
	uint16_t ramp_period;

	uint8_t displayed_error;
	bool displayed_heating;

	bool display_update_requested;
};

static struct menu_context menu;


static void int_to_ascii_align_right(char *buf, uint8_t align_to_digit,
				     int16_t val,
				     int16_t min_val, int16_t max_val,
				     char fill_char)
{
	uint8_t i;

	val = clamp(val, min_val, max_val);

	/* Fill all digits with NUL. Needed for shifting. */
	memset(buf, '\0', align_to_digit + 1u);

	/* Format the value. */
	itoa(val, buf, 10);

	/* Right-align the number, if requested. */
	if (align_to_digit > 0) {
		/* Right-shift until the rightmost digit is non-NUL. */
		while (buf[align_to_digit] == '\0') {
			for (i = align_to_digit; i > 0; i--)
				buf[i] = buf[i - 1];
			buf[0] = fill_char;
		}
	}
}

#define menu_putstr(dest, str)	strcpy((dest), (str))

static void menu_put_temp(char *disp, fixpt_t temp, const char *symbol)
{
	int16_t temp_int;

	temp_int = (int16_t)fixpt_to_int(temp);
	int_to_ascii_align_right(disp, 2,
				 temp_int,
				 (int16_t)CONTRTEMP_NEGLIM,
				 (int16_t)CONTRTEMP_POSLIM,
				 ' ');
	menu_putstr(disp + 3, symbol);
}

static void menu_put_fixpt(char *disp, fixpt_t val, uint8_t fract_digits)
{
	int16_t ipart, fpart;
	int16_t ipart_max, fpart_max;
	int16_t ipart_digits;
	uint8_t ipart_align, fpart_align;

	if (fract_digits == 2) {
		ipart_max = 99;
		ipart_align = 1;
		fpart_max = 99;
		fpart_align = 1;
	} else { /* fract_digits == 3 */
		ipart_max = 9;
		ipart_align = 0;
		fpart_max = 999;
		fpart_align = 2;
	}
	ipart_digits = 4 - fract_digits;

	ipart = (int16_t)clamp(fixpt_get_int_part(val), 0, ipart_max);
	fpart = (int16_t)fixpt_get_dec_fract(val, fract_digits);

	int_to_ascii_align_right(&disp[0], ipart_align, ipart,
				 0, ipart_max, ' ');
	disp[ipart_digits] = '.';
	int_to_ascii_align_right(&disp[ipart_digits + 1], fpart_align, fpart,
				 0, fpart_max, '0');
}

static void menu_update_display(void)
{
	char disp[6];
	const char *symbol;
	char _symbol[3];
	uint8_t displayed_error;
	uint8_t active_index;
	bool displayed_heating;
	enum contrtemp_boostmode boost_mode;

	boost_mode = contrtemp_get_boost_mode();
	displayed_error = menu.displayed_error;
	displayed_heating = menu.displayed_heating;
	disp[0] = '\0';

	if (displayed_error) {
		menu_putstr(disp, "Err ");
		disp[3] = (char)('0' + displayed_error);
		displayed_heating = false;
	} else {
		switch (menu.state) {
		case MENU_CURTEMP:
			if (contrtemp_is_idle()) {
				symbol = "L.";
			} else {
				switch (boost_mode) {
				case NR_BOOST_MODES:
				default:
				case TEMPBOOST_NORMAL:
					symbol = "C.";
					break;
#if CONF_BOOST
				case TEMPBOOST_BOOST1:
					symbol = "b.";
					break;
				case TEMPBOOST_BOOST2:
					symbol = "8.";
					break;
#endif
				}
			}
			menu_put_temp(disp, contrtemp_get_feedback(), symbol);
			break;
		case MENU_SELPRESET:
		case MENU_CHGPRESET:
			if (CONF_PRESETS) {
				active_index = presets_get_active_index();
				_symbol[0] = (char)('1' + active_index);
				_symbol[1] = '.';
				_symbol[2] = '\0';
				symbol = _symbol;
			} else
				symbol = " ";
			menu_put_temp(disp, presets_get_active_value(), symbol);
			break;
		case MENU_IDLETEMP:
			if (!CONF_IDLE)
				break;
			menu_put_temp(disp, get_settings()->temp_idle_setpoint, "L");
			break;
		case MENU_DEBUG:
			if (!CONF_DEBUG)
				break;
			menu_putstr(disp, "DBG");
			break;
		case MENU_CALIB:
			if (!CONF_CALIB)
				break;
			menu_putstr(disp, "CAL");
			break;
		case MENU_KP_PRE:
			if (!CONF_KCONF)
				break;
			menu_putstr(disp, " P");
			displayed_heating = false;
			break;
		case MENU_KP:
			if (!CONF_KCONF)
				break;
			menu_put_fixpt(disp, get_settings()->temp_k[boost_mode].kp, 2);
			displayed_heating = false;
			break;
		case MENU_KI_PRE:
			if (!CONF_KCONF)
				break;
			menu_putstr(disp, " I");
			displayed_heating = false;
			break;
		case MENU_KI:
			if (!CONF_KCONF)
				break;
			menu_put_fixpt(disp, get_settings()->temp_k[boost_mode].ki, 3);
			displayed_heating = false;
			break;
		case MENU_KD_PRE:
			if (!CONF_KCONF)
				break;
			menu_putstr(disp, " D");
			displayed_heating = false;
			break;
		case MENU_KD:
			if (!CONF_KCONF)
				break;
			menu_put_fixpt(disp, get_settings()->temp_k[boost_mode].kd, 2);
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
	menu.display_update_requested = true;
	mb();
}

static enum menu_state next_menu_state(enum menu_state cur_state)
{
	switch (cur_state) {
	case MENU_CURTEMP:
		if (CONF_IDLE)
			return MENU_IDLETEMP;
		if (CONF_DEBUG)
			return MENU_DEBUG;
		if (CONF_CALIB)
			return MENU_CALIB;
		if (CONF_KCONF)
			return MENU_KP_PRE;
		return MENU_CURTEMP;
	case MENU_SELPRESET:
	case MENU_CHGPRESET:
		return MENU_CURTEMP;
	case MENU_IDLETEMP:
		if (CONF_DEBUG)
			return MENU_DEBUG;
		if (CONF_CALIB)
			return MENU_CALIB;
		if (CONF_KCONF)
			return MENU_KP_PRE;
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
		if (CONF_KCONF)
			return MENU_KP;
		return MENU_CURTEMP;
	case MENU_KP:
		if (CONF_KCONF)
			return MENU_KI_PRE;
		return MENU_CURTEMP;
	case MENU_KI_PRE:
		if (CONF_KCONF)
			return MENU_KI;
		return MENU_CURTEMP;
	case MENU_KI:
		if (CONF_KCONF)
			return MENU_KD_PRE;
		return MENU_CURTEMP;
	case MENU_KD_PRE:
		if (CONF_KCONF)
			return MENU_KD;
		return MENU_CURTEMP;
	case MENU_KD:
		if (CONF_KCONF)
			return MENU_CURTEMP;
		return MENU_CURTEMP;
	}

	return MENU_CURTEMP;
}

static void menu_set_state(enum menu_state new_state)
{
	if (menu.state == new_state)
		return;
	menu.state = new_state;

	switch (new_state) {
	case MENU_SELPRESET:
		timer_arm(&menu.timeout, MENU_SELPRESET_TIMEOUT);
		break;
	case MENU_CHGPRESET:
		timer_arm(&menu.timeout, MENU_CHGPRESET_TIMEOUT);
		break;
	case MENU_IDLETEMP:
		timer_arm(&menu.timeout, MENU_IDLETEMP_TIMEOUT);
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

static fixpt_t do_ramp_temp(fixpt_t temp, bool up)
{
	return fixpt_add_limited(temp,
				 (up ? float_to_fixpt(1.0) :
				       float_to_fixpt(-1.0)),
				 float_to_fixpt(CONTRTEMP_NEGLIM),
				 float_to_fixpt(CONTRTEMP_POSLIM));
}

static void settemp_ramp_handler(bool up)
{
	fixpt_t setpoint;

	setpoint = presets_get_active_value();
	setpoint = do_ramp_temp(setpoint, up);
	presets_set_active_value(setpoint);

	menu_set_state(MENU_CHGPRESET);
}

static void idletemp_ramp_handler(bool up)
{
	fixpt_t setpoint;

	setpoint = get_settings()->temp_idle_setpoint;
	setpoint = do_ramp_temp(setpoint, up);
	contrtemp_set_idle_setpoint(setpoint);
}

static void do_ramp_k(fixpt_t *k, fixpt_t inc, fixpt_t max, bool up)
{
	*k = fixpt_add_limited(*k, (up ? inc : fixpt_neg(inc)),
			       float_to_fixpt(0.0), max);
	store_settings();
	contrtemp_update_pid_config();
}

static void kconf_kp_ramp_handler(bool up)
{
	do_ramp_k(&(get_settings()->temp_k[contrtemp_get_boost_mode()].kp),
		  float_to_fixpt(0.03125),
		  float_to_fixpt(99.0), up);
}

static void kconf_ki_ramp_handler(bool up)
{
	do_ramp_k(&(get_settings()->temp_k[contrtemp_get_boost_mode()].ki),
		  float_to_fixpt(0.015625),
		  float_to_fixpt(9.0), up);
}

static void kconf_kd_ramp_handler(bool up)
{
	do_ramp_k(&(get_settings()->temp_k[contrtemp_get_boost_mode()].kd),
		  float_to_fixpt(0.03125),
		  float_to_fixpt(99.0), up);
}

static void start_ramping(bool up, bool pre_ramp_wait, ramp_handler_t handler)
{
	if (pre_ramp_wait) {
		menu.ramp = up ? RAMP_PRE_UP : RAMP_PRE_DOWN;
		timer_arm(&menu.ramp_pre_timer, RAMP_PRE_TIMEOUT_MS);
	} else {
		menu.ramp = up ? RAMP_UP : RAMP_DOWN;
	}
	menu.ramp_handler = handler;
	menu.ramp_period = RAMP_START_PERIOD_MS;
	timer_set_now(&menu.ramp_timer);
}

static void start_ramping_button(enum button_id button,
				 enum button_state bstate,
				 bool pre_ramp_wait,
				 ramp_handler_t handler)
{
	if (bstate == BSTATE_POSEDGE) {
		if (button == BUTTON_MINUS)
			start_ramping(false, pre_ramp_wait, handler);
		else if (button == BUTTON_PLUS)
			start_ramping(true, pre_ramp_wait, handler);
	}
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
		return;
	}

	switch (menu.state) {
	case MENU_CURTEMP:
		if (bstate == BSTATE_POSEDGE) {
			start_ramping_button(button, bstate, (CONF_PRESETS),
					     settemp_ramp_handler);
			if (button != BUTTON_SET) {
				if (CONF_PRESETS)
					menu_set_state(MENU_SELPRESET);
				else
					menu_set_state(MENU_CHGPRESET);
			}
		}
		break;
	case MENU_SELPRESET:
		if (!CONF_PRESETS)
			break;
		timer_arm(&menu.timeout, MENU_SELPRESET_TIMEOUT);
		if (bstate == BSTATE_NEGEDGE) {
			if (button == BUTTON_PLUS)
				presets_next();
			if (button == BUTTON_MINUS)
				presets_prev();
		}
		start_ramping_button(button, bstate, true,
				     settemp_ramp_handler);
		break;
	case MENU_CHGPRESET:
		timer_arm(&menu.timeout, MENU_CHGPRESET_TIMEOUT);
		start_ramping_button(button, bstate, false,
				     settemp_ramp_handler);
		break;
	case MENU_IDLETEMP:
		if (!CONF_IDLE)
			break;
		timer_arm(&menu.timeout, MENU_IDLETEMP_TIMEOUT);
		start_ramping_button(button, bstate, false,
				     idletemp_ramp_handler);
		break;
	case MENU_DEBUG:
		if (!CONF_DEBUG)
			break;
		if (bstate == BSTATE_POSEDGE) {
			if (button == BUTTON_PLUS)
				debug_enable(true);
			if (button == BUTTON_MINUS) {
				debug_enable(false);
				menu_request_display_update();
			}
		}
		break;
	case MENU_CALIB:
		if (!CONF_CALIB)
			break;
		if (bstate == BSTATE_POSEDGE) {
			if (button != BUTTON_SET) {
				calcurr_set_enabled(!calcurr_is_enabled());
				menu_request_display_update();
			}
		}
		if (bstate == BSTATE_NEGEDGE) {
			if (button == BUTTON_SET)
				calcurr_set_enabled(false);
		}
		break;
	case MENU_KP_PRE:
		break;
	case MENU_KP:
		if (!CONF_KCONF)
			break;
		start_ramping_button(button, bstate, false,
				     kconf_kp_ramp_handler);
		break;
	case MENU_KI_PRE:
		break;
	case MENU_KI:
		if (!CONF_KCONF)
			break;
		start_ramping_button(button, bstate, false,
				     kconf_ki_ramp_handler);
		break;
	case MENU_KD_PRE:
		break;
	case MENU_KD:
		if (!CONF_KCONF)
			break;
		start_ramping_button(button, bstate, false,
				     kconf_kd_ramp_handler);
		break;
	}

	if (bstate == BSTATE_NEGEDGE) {
		/* Stop ramping, if any button is released. */
		stop_ramping();

		/* Switch to the next menu via SET. */
		if (button == BUTTON_SET) {
			if (!debug_is_enabled())
				menu_set_next_state();
		}
	}
}

/* Periodic work. */
void menu_work(void)
{
	enum ramp_state ramp;
	uint8_t error;
	bool heating;

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
	case MENU_SELPRESET:
	case MENU_CHGPRESET:
	case MENU_IDLETEMP:
		if (timer_expired(&menu.timeout)) {
			if (menu.state == MENU_IDLETEMP)
				menu_set_state(MENU_CURTEMP);
			else
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
	if (error)
		stop_ramping();

	/* Update heating condition. */
	heating = contrtemp_is_heating_up();
	if (heating != menu.displayed_heating) {
		menu.displayed_heating = heating;
		menu.display_update_requested = true;
	}

	/* Generic ramping handler. */
	ramp = menu.ramp;
	if (ramp != RAMP_NONE) {
		if (ramp == RAMP_PRE_UP ||
		    ramp == RAMP_PRE_DOWN) {
			if (timer_expired(&menu.ramp_pre_timer)) {
				start_ramping(ramp == RAMP_PRE_UP, false,
					      menu.ramp_handler);
			}
		} else {
			if (timer_expired(&menu.ramp_timer)) {
				menu.ramp_handler(ramp == RAMP_UP);
				menu.display_update_requested = true;
				timer_add(&menu.ramp_timer, menu.ramp_period);
				if (menu.ramp_period > RAMP_MIN_PERIOD_MS)
					menu.ramp_period /= 2;
			}
		}
	}

	/* Update the display, if requested. */
	mb();
	if (menu.display_update_requested) {
		menu.display_update_requested = false;
		mb();
		menu_update_display();
	}
}

void menu_init(void)
{
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
