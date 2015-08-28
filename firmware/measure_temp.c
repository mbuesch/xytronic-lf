/*
 * Xytronic LF-1600
 * Temperature measurement routines
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

#include "measure_temp.h"
#include "measure.h"
#include "timer.h"
#include "scale.h"
#include "controller_temp.h"


#define MEASTEMP_PERIOD_MS	100 /* ms */
#define MEASTEMP_CHANNEL	MEASCHAN_ADC1

/* Plausibility */
#define MEASTEMP_PLAUS_NEGLIM	20.0
#define MEASTEMP_PLAUS_POSLIM	480.0

/* Scaling */
#define MEASTEMP_SCALE_RAWLO	210
#define MEASTEMP_SCALE_PHYSLO	150.0
#define MEASTEMP_SCALE_RAWHI	411
#define MEASTEMP_SCALE_PHYSHI	480.0


static struct timer meastemp_timer;
static uint16_t meastemp_measured_raw;
static bool meastemp_is_plausible;


bool meastemp_value_is_plausible(void)
{
	return meastemp_is_plausible;
}

/* This runs in IRQ context. */
static void meastemp_meas_callback(void *context, uint16_t raw_adc)
{
	meastemp_measured_raw = raw_adc;
}

void meastemp_work(void)
{
	bool scheduled;
	uint16_t raw_adc;
	fixpt_t phys;
	uint8_t sreg;

	sreg = irq_disable_save();
	raw_adc = meastemp_measured_raw;
	meastemp_measured_raw = MEASURE_MAX_RESULT + 1;
	irq_restore(sreg);

	if (raw_adc <= MEASURE_MAX_RESULT) {
		phys = scale((int16_t)raw_adc,
			     MEASTEMP_SCALE_RAWLO,
			     MEASTEMP_SCALE_RAWHI,
			     float_to_fixpt(MEASTEMP_SCALE_PHYSLO),
			     float_to_fixpt(MEASTEMP_SCALE_PHYSHI));

		/* Plausibility check. */
		if (phys < float_to_fixpt(MEASTEMP_PLAUS_NEGLIM)) {
			phys = float_to_fixpt(MEASTEMP_PLAUS_NEGLIM);
			meastemp_is_plausible = false;
		} else if (phys > float_to_fixpt(MEASTEMP_PLAUS_POSLIM)) {
			phys = float_to_fixpt(MEASTEMP_PLAUS_POSLIM);
			meastemp_is_plausible = false;
		} else {
			meastemp_is_plausible = true;
		}

		contrtemp_set_feedback(phys);
	}

	if (timer_expired(&meastemp_timer)) {
		scheduled = measure_schedule(MEASTEMP_CHANNEL,
					     meastemp_meas_callback, NULL);
		if (scheduled)
			timer_add(&meastemp_timer, MEASTEMP_PERIOD_MS);
	}
}

void meastemp_init(void)
{
	timer_arm(&meastemp_timer, 0);
}
