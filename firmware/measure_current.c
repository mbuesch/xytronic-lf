/*
 * Xytronic LF-1600
 * Current measurement routines
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

#include "measure_current.h"
#include "measure.h"
#include "timer.h"
#include "scale.h"
#include "controller_current.h"
#include "debug_uart.h"


#define MEASCURR_PERIOD_MS	100 /* ms */
#define MEASCURR_CHANNEL	MEASCHAN_ADC2

/* Plausibility */
#define MEASCURR_PLAUS_NEGLIM	0.0
#define MEASCURR_PLAUS_POSLIM	5.0

/* Scaling */
#define MEASCURR_SCALE_RAWLO	80
#define MEASCURR_SCALE_PHYSLO	2.5
#define MEASCURR_SCALE_RAWHI	140
#define MEASCURR_SCALE_PHYSHI	4.0


static struct timer meascurr_timer;
static uint16_t meascurr_measured_raw;
static bool meascurr_is_plausible;
static struct report_int16_context meascurr_report;


bool meascurr_value_is_plausible(void)
{
	return meascurr_is_plausible;
}

/* This runs in IRQ context. */
static void meascurr_meas_callback(void *context, uint16_t raw_adc)
{
	meascurr_measured_raw = raw_adc;
}

void meascurr_work(void)
{
	bool scheduled;
	uint16_t raw_adc;
	fixpt_t phys;
	uint8_t sreg;

	sreg = irq_disable_save();
	raw_adc = meascurr_measured_raw;
	meascurr_measured_raw = MEASURE_MAX_RESULT + 1;
	irq_restore(sreg);

	if (raw_adc <= MEASURE_MAX_RESULT) {
		debug_report_int16(&meascurr_report, PSTR("mc"),
				   (int16_t)raw_adc);

		phys = scale((int16_t)raw_adc,
			     MEASCURR_SCALE_RAWLO,
			     MEASCURR_SCALE_RAWHI,
			     float_to_fixpt(MEASCURR_SCALE_PHYSLO),
			     float_to_fixpt(MEASCURR_SCALE_PHYSHI));

		/* Plausibility check. */
		if (phys < float_to_fixpt(MEASCURR_PLAUS_NEGLIM)) {
			phys = float_to_fixpt(MEASCURR_PLAUS_NEGLIM);
			meascurr_is_plausible = false;
		} else if (phys > float_to_fixpt(MEASCURR_PLAUS_POSLIM)) {
			phys = float_to_fixpt(MEASCURR_PLAUS_POSLIM);
			meascurr_is_plausible = false;
		} else {
			meascurr_is_plausible = true;
		}

		contrcurr_set_feedback(phys);
	}

	if (timer_expired(&meascurr_timer)) {
		scheduled = measure_schedule(MEASCURR_CHANNEL,
					     meascurr_meas_callback, NULL);
		if (scheduled)
			timer_add(&meascurr_timer, MEASCURR_PERIOD_MS);
	}
}

void meascurr_init(void)
{
	timer_arm(&meascurr_timer, 0);
}
