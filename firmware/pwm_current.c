/*
 * Xytronic LF-1600
 * Current PWM
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

#include "pwm_current.h"
#include "scale.h"
#include "controller_current.h"


#define PWMCURR_MAX_DUTY	0x3FF


void pwmcurr_set(fixpt_t current_amps)
{
	int16_t duty;

	duty = unscale(current_amps,
		       float_to_fixpt(CONTRCURR_NEGLIM),
		       float_to_fixpt(CONTRCURR_POSLIM),
		       0, PWMCURR_MAX_DUTY);

	//TODO
}

void pwmcurr_work(void)
{
	//TODO
}

void pwmcurr_init(void)
{
	//TODO
}
