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


#define MEASCURR_PERIOD_MS	100 /* ms */
#define MEASCURR_CHANNEL	MEASCHAN_ADC2


static struct timer meascurr_timer;


/* This runs in IRQ context. */
static void meascurr_meas_callback(void *context, uint16_t raw_adc)
{
	fixpt_t phys;

	phys = scale((int16_t)raw_adc, 0, MEASURE_MAX_RESULT,
		     float_to_fixpt(CONTRCURR_NEGLIM), //FIXME limits are not ok
		     float_to_fixpt(CONTRCURR_POSLIM));
	contrcurr_set_feedback(phys);
}

void meascurr_work(void)
{
	bool scheduled;

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
