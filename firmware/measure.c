/*
 * Xytronic LF-1600
 * Measurement routines
 *
 * Copyright (c) 2015-2016 Michael Buesch <m@bues.ch>
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
#include "scale.h"
#include "timer.h"
#include "debug_uart.h"
#include "ring.h"

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

	int16_t old_report_value;

	bool plaus_timeout;
	bool plaus_timeout_timer_running;
	struct timer plaus_timeout_timer;
};

struct meas_context {
	struct meas_chan_context channels[NR_MEAS_CHANS];
	uint8_t active_chan;

	uint16_t avg_count;
	uint32_t avg_sum;
	struct timer avg_timer;

	bool result_available;
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
		timer_arm(&meas.avg_timer,
			  config->averaging_timeout_ms);
		adc_trigger(config->mux, config->ps, config->ref,
			    true, true);
	} else {
		adc_trigger(MEAS_MUX_GND, MEAS_PS_64, MEAS_REF_AREF,
			    true, false);
	}
}

static void adc_trigger_next_chan(void)
{
	uint8_t active_chan;

	/* Switch to the next channel. */
	active_chan = ring_next(meas.active_chan, ARRAY_SIZE(meas.channels) - 1u);
	meas.active_chan = active_chan;
	adc_trigger_chan(&meas.channels[meas.active_chan]);
}

static void measure_handle_result(void)
{
	struct meas_chan_context *active_chan;
	const struct measure_config __flash *config;
	uint16_t raw_adc;
	fixpt_t phys;
	bool is_plausible;
	enum measure_plausibility plaus;

	active_chan = &meas.channels[meas.active_chan];
	config = active_chan->config;

	/* Calculate the result of the averaging. */
	raw_adc = (uint16_t)(meas.avg_sum / (uint32_t)meas.avg_count);

	debug_report_int16(config->name, &active_chan->old_report_value,
			   (int16_t)raw_adc);

	/* Filter the raw adc value, if we have a filter. */
	//TODO we should filter the phys value instead of the raw value, so that we only need a fixpt implementation of the filter.
	if (config->filter_callback)
		raw_adc = config->filter_callback(raw_adc);

	/* Scale raw to phys. */
	phys = scale((int16_t)raw_adc,
		     (int16_t)config->scale_raw_lo,
		     (int16_t)config->scale_raw_hi,
		     config->scale_phys_lo,
		     config->scale_phys_hi);

	/* Plausibility check. */
	is_plausible = true;
	if (phys < config->plaus_neglim) {
		phys = config->plaus_neglim;
		is_plausible = false;
	} else if (phys > config->plaus_poslim) {
		phys = config->plaus_poslim;
		is_plausible = false;
	}

	if (is_plausible) {
		active_chan->plaus_timeout_timer_running = false;
		active_chan->plaus_timeout = false;
	} else {
		if (!active_chan->plaus_timeout_timer_running) {
			timer_arm(&active_chan->plaus_timeout_timer,
				  config->plaus_timeout_ms);
			active_chan->plaus_timeout_timer_running = true;
		}
	}

	if (active_chan->plaus_timeout_timer_running &&
	    !active_chan->plaus_timeout &&
	    timer_expired(&active_chan->plaus_timeout_timer))
		active_chan->plaus_timeout = true;

	/* Call the result callback. */
	if (is_plausible) {
		plaus = MEAS_PLAUSIBLE;
	} else {
		if (active_chan->plaus_timeout)
			plaus = MEAS_PLAUS_TIMEOUT;
		else
			plaus = MEAS_NOT_PLAUSIBLE;
	}
	config->result_callback(phys, plaus);
}

ISR(ADC_vect)
{
	struct meas_chan_context *active_chan;
	const struct measure_config __flash *config;
	uint16_t raw_adc;

	mb();

	/* Read the raw ADC value. */
	raw_adc = ADC;

	active_chan = &meas.channels[meas.active_chan];
	config = active_chan->config;

	if (config) {
		/* Add it to the averaging sum and check if we are done. */
		meas.avg_sum += raw_adc;
		meas.avg_count += 1;
		if (timer_expired(&meas.avg_timer)) {
			meas.result_available = true;
			adc_disable();
		}
	} else {
		adc_trigger_next_chan();
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

void measure_work(void)
{
	uint8_t sreg;
	bool result_available;

	sreg = irq_disable_save();
	result_available = meas.result_available;
	meas.result_available = false;
	irq_restore(sreg);

	if (result_available) {
		measure_handle_result();
		adc_trigger_next_chan();
	}
}

void measure_init(void)
{
	/* Discard the first measurement. */
	adc_trigger_chan(NULL);
	adc_busywait();
	(void)ADC;
	adc_disable();
}
