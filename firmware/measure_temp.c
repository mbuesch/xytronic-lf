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
#include "controller_current.h"
#include "debug_uart.h"

#include <string.h>


struct meastemp_context {
	fixpt_t prev_feedback;
};


static struct meastemp_context meastemp;


static void meastemp_result_callback(fixpt_t measured_phys_value,
				     enum measure_plausibility plaus)
{
	/* Set/reset emergency status. */
	if (plaus == MEAS_PLAUSIBLE)
		contrtemp_set_emerg(false);
	else if (plaus == MEAS_PLAUS_TIMEOUT)
		contrtemp_set_emerg(true);

	/* Set the controller feedback. */
	if (plaus == MEAS_PLAUSIBLE) {
		meastemp.prev_feedback = measured_phys_value;
		contrcurr_set_restricted(
			measured_phys_value < float_to_fixpt(CONTRCURR_RESTRICT_TOTEMP)
		);
		contrtemp_set_feedback(measured_phys_value);
	} else if (plaus == MEAS_NOT_PLAUSIBLE) {
		/* Just run the controller with the previous feedback. */
		contrtemp_set_feedback(meastemp.prev_feedback);
	}
}

static const struct measure_config __flash meastemp_config = {
	.name			= "mt",
	.mux			= MEAS_MUX_ADC1,
	.ps			= MEAS_PS_64,
	.ref			= MEAS_REF_AREF,
	.averaging_timeout_ms	= 50,
	.scale_raw_lo		= 210,
	.scale_raw_hi		= 411,
	.scale_phys_lo		= FLOAT_TO_FIXPT(150.0),
	.scale_phys_hi		= FLOAT_TO_FIXPT(480.0),
	.plaus_neglim		= FLOAT_TO_FIXPT(20.0),
	.plaus_poslim		= FLOAT_TO_FIXPT(480.0),
	.plaus_timeout_ms	= 1000,
	.filter_callback	= NULL,
	.result_callback	= meastemp_result_callback,
};

void meastemp_init(void)
{
	memset(&meastemp, 0, sizeof(meastemp));
	measure_register_channel(MEAS_CHAN_1, &meastemp_config);
}
