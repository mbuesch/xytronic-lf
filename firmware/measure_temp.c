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
#include "debug_uart.h"

/* Plausibility */
#define MEASTEMP_PLAUS_NEGLIM	20.0
#define MEASTEMP_PLAUS_POSLIM	480.0

/* Scaling */
#define MEASTEMP_SCALE_RAWLO	210
#define MEASTEMP_SCALE_PHYSLO	150.0
#define MEASTEMP_SCALE_RAWHI	411
#define MEASTEMP_SCALE_PHYSHI	480.0


static bool meastemp_is_plausible;


//FIXME
bool meastemp_value_is_plausible(void)
{
	return meastemp_is_plausible;
}

/* This runs in IRQ context. */
static void meastemp_meas_callback(fixpt_t measured_phys_value,
				   bool is_plausible)
{
	if (is_plausible) {
		contrtemp_set_feedback(measured_phys_value);
	} else {
		//TODO
	}
}

static const struct measure_config __flash meastemp_config = {
	.name			= "mt",
	.mux			= MEAS_MUX_ADC1,
	.ps			= MEAS_PS_64,
	.ref			= MEAS_REF_AREF,
	.averaging_count	= 128,
	.scale_raw_lo		= MEASTEMP_SCALE_RAWLO,
	.scale_raw_hi		= MEASTEMP_SCALE_RAWHI,
	.scale_phys_lo		= FLOAT_TO_FIXPT(MEASTEMP_SCALE_PHYSLO),
	.scale_phys_hi		= FLOAT_TO_FIXPT(MEASTEMP_SCALE_PHYSHI),
	.plaus_neglim		= FLOAT_TO_FIXPT(MEASTEMP_PLAUS_NEGLIM),
	.plaus_poslim		= FLOAT_TO_FIXPT(MEASTEMP_PLAUS_POSLIM),
	.callback		= meastemp_meas_callback,
};

void meastemp_init(void)
{
	measure_register_channel(MEAS_CHAN_1, &meastemp_config);
}
