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


static void meastemp_meas_callback(fixpt_t measured_phys_value,
				   enum measure_plausibility plaus)
{
	switch (plaus) {
	case MEAS_PLAUSIBLE:
		contrtemp_set_emerg(false);
		contrtemp_set_feedback(measured_phys_value);
		contrcurr_set_restricted(
			measured_phys_value < float_to_fixpt(CONTRCURR_RESTRICT_TOTEMP)
		);
		break;
	case MEAS_NOT_PLAUSIBLE:
		break;
	case MEAS_PLAUS_TIMEOUT:
		contrtemp_set_emerg(true);
		break;
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
	.callback		= meastemp_meas_callback,
};

void meastemp_init(void)
{
	measure_register_channel(MEAS_CHAN_1, &meastemp_config);
}
