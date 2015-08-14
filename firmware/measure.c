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
#include <avr/interrupt.h>


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
static measure_cb_t active_cb;
static void *active_cb_context;


ISR(ADC_vect)
{
	uint16_t raw_adc;

	mb();

	/* Read the raw ADC value. */
	raw_adc = ADC;

	/* If a callback is defined, call it. */
	if (active_cb) {
		active_cb(active_cb_context, raw_adc);
		active_cb = NULL;
		active_cb_context = NULL;
	}

	/* Mark the ADC resource as unused. */
	active_mux = MUX_NONE;

	mb();
}

static void adc_trigger(uint8_t mux, uint8_t ps, bool irq)
{
	uint8_t ref;

	/* Mask the multiplexer bits. */
	mux &= (1 << MUX3) | (1 << MUX2) | (1 << MUX1) | (1 << MUX0);
	/* Mask the prescaler bits. */
	ps &= (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);

	/* Mark the ADC resource as used. */
	active_mux = mux;
	mb();

	/* REF = AREF pin */
	ref = (0 << REFS1) | (0 << REFS0);

	/* Set multiplexer and start conversion. */
	ADMUX = ref | (0 << ADLAR) | mux;
	if (irq) {
		ADCSRA = (1 << ADEN) | (1 << ADSC) |
			 (1 << ADIE) | (0 << ADATE) | ps;
	} else {
		ADCSRA = (1 << ADEN) | (1 << ADSC) |
			 (0 << ADIE) | (0 << ADATE) | ps;
	}
}

static void adc_busywait(void)
{
	do { } while (ADCSRA & (1 << ADSC));
	ADCSRA |= (1 << ADIF); /* Clear IRQ flag */
}

bool measure_schedule(enum measure_chan chan,
		      measure_cb_t callback, void *context)
{
	uint8_t sreg;
	bool success;
	uint8_t mux;
	uint8_t ps;

	switch (chan) {
	case MEASCHAN_ADC0:
		mux = MUX_ADC0;
		break;
	case MEASCHAN_ADC1:
		mux = MUX_ADC1;
		break;
	case MEASCHAN_ADC2:
		mux = MUX_ADC2;
		break;
	case MEASCHAN_ADC3:
		mux = MUX_ADC3;
		break;
	case MEASCHAN_ADC4:
		mux = MUX_ADC4;
		break;
	case MEASCHAN_ADC5:
		mux = MUX_ADC5;
		break;
	default:
		return false;
	}

	ps = PS_128;

	sreg = irq_disable_save();
	if (active_mux == MUX_NONE) {
		active_cb = callback;
		active_cb_context = context;
		adc_trigger(mux, ps, true);

		success = true;
	} else {
		success = false;
	}
	irq_restore(sreg);

	return success;
}

void measure_init(void)
{
	active_cb = NULL;
	active_cb_context = NULL;

	/* Discard the first measurement. */
	adc_trigger(MUX_GND, PS_128, false);
	adc_busywait();
	(void)ADC;

	/* Mark the ADC resource as unused. */
	active_mux = MUX_NONE;
}
