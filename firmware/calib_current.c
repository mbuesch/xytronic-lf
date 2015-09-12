/*
 * Xytronic LF-1600
 * Current calibration helper routines
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

#include "calib_current.h"
#include "controller_temp.h"
#include "controller_current.h"
#include "measure_current.h"
#include "debug_uart.h"


#define CALCURR_TEMP_HI		250.0
#define CALCURR_TEMP_LO		75.0


struct calcurr_context {
	bool enabled;
	uint8_t pwm_percent;
	bool heating_up;
};


static struct calcurr_context calcurr;


static void calcurr_set_percent(uint8_t percent)
{
	calcurr.pwm_percent = percent;
	debug_print_int16(PSTR("cc"), percent);
}

void calcurr_work(void)
{
	fixpt_t temp_r;

	if (!calcurr.enabled)
		return;

	temp_r = contrtemp_get_feedback();
	if (calcurr.heating_up) {
		if (temp_r >= float_to_fixpt(CALCURR_TEMP_HI)) {
			calcurr.heating_up = false;
			if (calcurr.pwm_percent >= 100) {
				calcurr_set_enabled(false);
			} else {
				calcurr_set_percent((uint8_t)(calcurr.pwm_percent + 10));
				contrcurr_set_enabled(false, 0);
			}
		}
	} else {
		if (temp_r <= float_to_fixpt(CALCURR_TEMP_LO)) {
			calcurr.heating_up = true;
			meascurr_filter_reset();
			contrcurr_set_enabled(false, calcurr.pwm_percent);
		}
	}
}

bool calcurr_is_enabled(void)
{
	return calcurr.enabled;
}

void calcurr_set_enabled(bool enable)
{
	calcurr.enabled = enable;
	calcurr.heating_up = false;

	contrtemp_set_enabled(!enable);
	contrcurr_set_enabled(!enable, 0);
	debug_enable(enable);
	if (enable)
		calcurr_set_percent(10);
}
