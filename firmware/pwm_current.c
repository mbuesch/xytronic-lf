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
#include "timer.h"
#include "scale.h"
#include "controller_current.h"
#include "util.h"

#include <avr/io.h>


/* The max duty value (which actually is min current).
 * Note that changing this changes the PWM frequency.
 */
#define PWMCURR_MAX_DUTY	0x1FFF

/* The ramping step size. */
#define PWMCURR_RAMP_STEP	PWMCURR_MAX_DUTY


static struct timer pwmcurr_timer;
static uint16_t pwmcurr_desired_ocr;


static void pwmcurr_ocr_ramp_handle(void)
{
	uint16_t cur_ocr;
	int16_t diff;
	int16_t step;

	cur_ocr = OCR1A;
	diff = (int16_t)pwmcurr_desired_ocr - (int16_t)cur_ocr;
	if (diff >= 0)
		step = min((int16_t)PWMCURR_RAMP_STEP, diff);
	else
		step = max(-((int16_t)PWMCURR_RAMP_STEP), diff);
	OCR1A = (uint16_t)((int16_t)cur_ocr + step);
}

void pwmcurr_set(fixpt_t current_amps)
{
	uint16_t duty;

	/* Scale amps to duty cycle duration.
	 * Scaling is inverse:
	 * 0 -> full current
	 * pwm-max -> no current
	 */
	duty = (uint16_t)unscale(current_amps,
				 float_to_fixpt(CONTRCURR_NEGLIM),
				 float_to_fixpt(CONTRCURR_POSLIM),
				 (int16_t)PWMCURR_MAX_DUTY, 0);

	/* Clamp the result.
	 * This should not be necessary, but better be safe.
	 */
	duty = clamp(duty, (uint16_t)0, (uint16_t)PWMCURR_MAX_DUTY);

	/* Program the hardware */
	pwmcurr_desired_ocr = duty;
	pwmcurr_ocr_ramp_handle();
}

void pwmcurr_work(void)
{
	if (!timer_expired(&pwmcurr_timer))
		return;
	timer_add(&pwmcurr_timer, 1);

	pwmcurr_ocr_ramp_handle();
}

void pwmcurr_init(void)
{
	build_assert(F_CPU == 8000000UL);

	/* Configure the port as output; high level. */
	PORTB |= (1 << PB1);
	DDRB |= (1 << DDB1);

	/* Disable timer */
	TCCR1B = 0;

	/* Set frequency and initial duty cycle. */
	ICR1 = PWMCURR_MAX_DUTY;
	OCR1A = PWMCURR_MAX_DUTY;
	pwmcurr_desired_ocr = PWMCURR_MAX_DUTY;
	OCR1B = 0;
	TCNT1 = 0;
	/* Disable all interrupts */
	TIMSK1 &= (uint8_t)~((1 << ICIE1) |
			     (1 << OCIE1B) | (1 << OCIE1A) |
			     (1 << TOIE1));
	/* Enable timer: Fast PWM (Mode 14), OC1A clr/set, PS 64 */
	TCCR1A = (1 << COM1A1) | (0 << COM1A0) |
		 (0 << COM1B1) | (0 << COM1B0) |
		 (1 << WGM11) | (0 << WGM10);
	TCCR1C = 0;
	TCCR1B = (1 << WGM13) | (1 << WGM12) |
		 (0 << CS12) | (1 << CS11) | (1 << CS10);

	timer_arm(&pwmcurr_timer, 0);
}
