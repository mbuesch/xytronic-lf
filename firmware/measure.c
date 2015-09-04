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

#include <string.h>

#include <avr/io.h>
#include <avr/interrupt.h>


/* Trigger source selection */
#define TS_FREERUN		((0 << ADTS2) | (0 << ADTS1) | (0 << ADTS0))
#define TS_ACOMP		((0 << ADTS2) | (0 << ADTS1) | (1 << ADTS0))
#define TS_INT0			((0 << ADTS2) | (1 << ADTS1) | (0 << ADTS0))
#define TS_T0CMA		((0 << ADTS2) | (1 << ADTS1) | (1 << ADTS0))
#define TS_T0OV			((1 << ADTS2) | (0 << ADTS1) | (0 << ADTS0))
#define TS_T1CMB		((1 << ADTS2) | (0 << ADTS1) | (1 << ADTS0))
#define TS_T1OV			((1 << ADTS2) | (1 << ADTS1) | (0 << ADTS0))
#define TS_T1CAP		((1 << ADTS2) | (1 << ADTS1) | (1 << ADTS0))


struct meas_chan_context {
	const struct measure_config __flash *config;
};

struct meas_context {
	struct meas_chan_context channels[NR_MEAS_CHANS];
	uint8_t active_chan;
	uint16_t avg_count;
	uint32_t avg_sum;
};

static struct meas_context meas;


static void adc_disable(void)
{
	/* Disable the ADC. */
	ADCSRA = 0;
	/* Clear the interrupt flag. */
	ADCSRA |= 1 << ADIF;
}

static void adc_busywait(void)
{
	do { } while (ADCSRA & (1 << ADSC));
}

static void adc_trigger(uint8_t mux, uint8_t ps, uint8_t ref,
			bool irq, bool freerunning)
{
	uint8_t trig, ie;

	/* Mask the multiplexer bits. */
	mux &= (1 << MUX3) | (1 << MUX2) | (1 << MUX1) | (1 << MUX0);
	/* Mask the prescaler bits. */
	ps &= (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
	/* Mask the reference bits. */
	ref &= (1 << REFS1) | (1 << REFS0);

	/* Free-running mode selection. */
	if (freerunning)
		trig = 1 << ADATE;
	else
		trig = 0 << ADATE;

	/* Interrupt-enable selection */
	if (irq)
		ie = 1 << ADIE;
	else
		ie = 0 << ADIE;

	mb();

	/* Set multiplexer and start conversion. */
	ADMUX = ref | (0 << ADLAR) | mux;
	ADCSRB = (0 << ACME) | TS_FREERUN;
	ADCSRA = (1 << ADEN) | (1 << ADSC) | ps | ie | trig;
}

static void adc_trigger_chan(struct meas_chan_context *chan)
{
	const struct measure_config __flash *config = chan ? chan->config : NULL;

	adc_disable();
	meas.avg_sum = 0;
	meas.avg_count = 0;
	if (config) {
		adc_trigger(config->mux, config->ps, config->ref,
			    true, true);
	} else {
		adc_trigger(MEAS_MUX_GND, MEAS_PS_64, MEAS_REF_AREF,
			    true, false);
	}
}

ISR(ADC_vect)
{
	struct meas_chan_context *active_chan;
	const struct measure_config __flash *config;
	uint16_t raw_adc;
	bool trigger_next_chan = false;

	mb();

	/* Read the raw ADC value. */
	raw_adc = ADC;

	active_chan = &meas.channels[meas.active_chan];
	config = active_chan->config;

	if (config) {
		/* Add it to the average and check if we are done. */
		meas.avg_sum += raw_adc;
		meas.avg_count += 1;
		if (meas.avg_count >= config->averaging_count) {
			raw_adc = (uint16_t)(meas.avg_sum / (uint32_t)meas.avg_count);

			/* Call the callback. */
			if (config->callback)
				config->callback(raw_adc);

			trigger_next_chan = true;
		}
	} else {
		trigger_next_chan = true;
	}
	if (trigger_next_chan) {
		/* Switch to the next channel. */
		meas.active_chan++;
		if (meas.active_chan >= ARRAY_SIZE(meas.channels))
			meas.active_chan = 0;
		adc_trigger_chan(&meas.channels[meas.active_chan]);
	}

	mb();
}

void measure_register_channel(enum measure_chan chan,
			      const struct measure_config __flash *config)
{
	meas.channels[chan].config = config;
}

void measure_start(void)
{
	adc_trigger_chan(&meas.channels[0]);
}

void measure_init(void)
{
	uint8_t i;

	memset(&meas, 0, sizeof(meas));
	for (i = 0; i < ARRAY_SIZE(meas.channels); i++)
		meas.channels[i].config = NULL;

	/* Discard the first measurement. */
	adc_trigger_chan(NULL);
	adc_busywait();
	(void)ADC;
	adc_disable();
}
