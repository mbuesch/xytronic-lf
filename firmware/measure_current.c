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


/* Plausibility */
#define MEASCURR_PLAUS_NEGLIM	0.0
#define MEASCURR_PLAUS_POSLIM	5.0

/* Scaling */
#define MEASCURR_SCALE_RAWLO	80
#define MEASCURR_SCALE_PHYSLO	2.5
#define MEASCURR_SCALE_RAWHI	140
#define MEASCURR_SCALE_PHYSHI	4.0


static bool meascurr_is_plausible;


//FIXME
bool meascurr_value_is_plausible(void)
{
	return meascurr_is_plausible;
}

/* This runs in IRQ context. */
static void meascurr_meas_callback(fixpt_t measured_phys_value,
				   bool is_plausible)
{
	if (is_plausible) {
measured_phys_value = fixpt_div(measured_phys_value, int_to_fixpt(6));//FIXME
		contrcurr_set_feedback(measured_phys_value);
	} else {
		//TODO
	}
}

static const struct measure_config __flash meascurr_config = {
	.name			= "mc",
	.mux			= MEAS_MUX_ADC2,
	.ps			= MEAS_PS_64,
	.ref			= MEAS_REF_AREF,
	.averaging_count	= 3000,
	.scale_raw_lo		= MEASCURR_SCALE_RAWLO,
	.scale_raw_hi		= MEASCURR_SCALE_RAWHI,
	.scale_phys_lo		= FLOAT_TO_FIXPT(MEASCURR_SCALE_PHYSLO),
	.scale_phys_hi		= FLOAT_TO_FIXPT(MEASCURR_SCALE_PHYSHI),
	.plaus_neglim		= FLOAT_TO_FIXPT(MEASCURR_PLAUS_NEGLIM),
	.plaus_poslim		= FLOAT_TO_FIXPT(MEASCURR_PLAUS_POSLIM),
	.callback		= meascurr_meas_callback,
};

void meascurr_init(void)
{
	measure_register_channel(MEAS_CHAN_0, &meascurr_config);
}
