/*
 * Xytronic LF-1600
 * Measurement routines
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

#include "measure.h"

#include <avr/io.h>


/* Multiplexer selections */
#define MUX_ADC0		((0 << MUX3) | (0 << MUX2) | (0 << MUX1) | (0 << MUX0))
#define MUX_ADC1		((0 << MUX3) | (0 << MUX2) | (0 << MUX1) | (1 << MUX0))
#define MUX_ADC2		((0 << MUX3) | (0 << MUX2) | (1 << MUX1) | (0 << MUX0))
#define MUX_ADC3		((0 << MUX3) | (0 << MUX2) | (1 << MUX1) | (1 << MUX0))
#define MUX_ADC4		((0 << MUX3) | (1 << MUX2) | (0 << MUX1) | (0 << MUX0))
#define MUX_ADC5		((0 << MUX3) | (1 << MUX2) | (0 << MUX1) | (1 << MUX0))
#define MUX_ADC6		((0 << MUX3) | (1 << MUX2) | (1 << MUX1) | (0 << MUX0))
#define MUX_ADC7		((0 << MUX3) | (1 << MUX2) | (1 << MUX1) | (1 << MUX0))
#define MUX_ADC8		((1 << MUX3) | (0 << MUX2) | (0 << MUX1) | (0 << MUX0))
#define MUX_BG			((1 << MUX3) | (1 << MUX2) | (1 << MUX1) | (0 << MUX0))
#define MUX_GND			((1 << MUX3) | (1 << MUX2) | (1 << MUX1) | (1 << MUX0))
#define MUX_NONE		0xFF

/* Prescaler selections */
#define PS_2			((0 << ADPS2) | (0 << ADPS1) | (1 << ADPS0))
#define PS_4			((0 << ADPS2) | (1 << ADPS1) | (0 << ADPS0))
#define PS_8			((0 << ADPS2) | (1 << ADPS1) | (1 << ADPS0))
#define PS_16			((1 << ADPS2) | (0 << ADPS1) | (0 << ADPS0))
#define PS_32			((1 << ADPS2) | (0 << ADPS1) | (1 << ADPS0))
#define PS_64			((1 << ADPS2) | (1 << ADPS1) | (0 << ADPS0))
#define PS_128			((1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0))


static uint8_t active_mux;


static void adc_trigger(uint8_t mux, uint8_t ps)
{
	active_mux = mux;
	ADMUX = (1 << REFS0) | mux;
	ADCSRA = (1 << ADEN) | (0 << ADIE) | (1 << ADSC) | (0 << ADATE) | ps;
}

static void adc_busywait(void)
{
	do { } while (ADCSRA & (1 << ADSC));
	ADCSRA |= (1 << ADIF); /* Clear IRQ flag */
}

void measure_init(void)
{
	/* Discard the first measurement. */
	adc_trigger(MUX_GND, PS_8);
	adc_busywait();
	(void)ADC;
	active_mux = MUX_NONE;
}
