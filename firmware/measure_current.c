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

#include <string.h>


/* Low pass filter time. */
#define MEASCURR_FILTER_SHIFT		4


struct meascurr_context {
	fixpt_t prev_feedback;
	uint24_t filter_buf;
	int16_t old_filter_report_value;
};


static struct meascurr_context meascurr;


void meascurr_filter_reset(void)
{
	meascurr.filter_buf = 0u;
}

static uint16_t meascurr_filter_callback(uint16_t raw_adc)
{
	uint24_t buf, out;
	uint16_t filtered_adc;

	/* Run a simple low pass filter. */
	buf = meascurr.filter_buf;
	buf -= buf >> MEASCURR_FILTER_SHIFT;
	buf += raw_adc;
	meascurr.filter_buf = buf;
	out = buf >> MEASCURR_FILTER_SHIFT;

	filtered_adc = (uint16_t)clamp(out, 0u, MEASURE_MAX_RESULT);

	debug_report_int16(PSTR("fc"), &meascurr.old_filter_report_value,
			   (int16_t)filtered_adc);

	return filtered_adc;
}

static void meascurr_result_callback(fixpt_t measured_phys_value,
				     enum measure_plausibility plaus)
{
	/* Set/reset emergency status. */
	if (plaus == MEAS_PLAUSIBLE)
		contrcurr_set_emerg(false);
	else if (plaus == MEAS_PLAUS_TIMEOUT)
		contrcurr_set_emerg(true);

	/* Set the controller feedback. */
	if (plaus == MEAS_PLAUSIBLE) {
		meascurr.prev_feedback = measured_phys_value;
		contrcurr_set_feedback(measured_phys_value);
	} else if (plaus == MEAS_NOT_PLAUSIBLE) {
		/* Just run the controller with the previous feedback. */
		contrcurr_set_feedback(meascurr.prev_feedback);
	}
}

static const struct measure_config __flash meascurr_config = {
	.name			= "mc",
	.mux			= MEAS_MUX_ADC2,
	.ps			= MEAS_PS_64,
	.ref			= MEAS_REF_AREF,
	.averaging_timeout_ms	= 5,
	.scale_raw_lo		= 0,
	.scale_raw_hi		= 85,
	.scale_phys_lo		= FLOAT_TO_FIXPT(0.0),
	.scale_phys_hi		= FLOAT_TO_FIXPT(1.5),
	.plaus_neglim		= FLOAT_TO_FIXPT(CONTRCURR_NEGLIM),
	.plaus_poslim		= FLOAT_TO_FIXPT(CONTRCURR_POSLIM),
	.plaus_timeout_ms	= 3000,
	.filter_callback	= meascurr_filter_callback,
	.result_callback	= meascurr_result_callback,
};

void meascurr_init(void)
{
	memset(&meascurr, 0, sizeof(meascurr));
	measure_register_channel(MEAS_CHAN_0, &meascurr_config);
}
