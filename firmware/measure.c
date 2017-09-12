/*
 * Xytronic LF-1600
 * Measurement routines
 *
 * Copyright (c) 2015-2017 Michael Buesch <m@bues.ch>
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

#if CONF_ADJ
	fixpt_t adjustment;
#endif

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

	mb();

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
	if (config->filter_callback)
		raw_adc = config->filter_callback(raw_adc);

	/* Scale raw to phys. */
	phys = scale((int16_t)raw_adc,
		     (int16_t)config->scale_raw_lo,
		     (int16_t)config->scale_raw_hi,
		     config->scale_phys_lo,
		     config->scale_phys_hi);

	/* Apply the physical value adjustment. */
#if CONF_ADJ
	phys = fixpt_sub(phys, active_chan->adjustment);
#endif

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
	uint8_t mux;

	/* Register the channel. */
	meas.channels[chan].config = config;

#if CONF_ADJ
	meas.channels[chan].adjustment = int_to_fixpt(0);
#endif

	/* Disable digital input on the pin. */
	mux = config->mux;
	if (mux == MEAS_MUX_ADC0 ||
	    mux == MEAS_MUX_ADC1 ||
	    mux == MEAS_MUX_ADC2 ||
	    mux == MEAS_MUX_ADC3 ||
	    mux == MEAS_MUX_ADC4 ||
	    mux == MEAS_MUX_ADC5) {
		static const uint8_t mux2didr[] = {
			[MEAS_MUX_ADC0] = 1u << ADC0D,
			[MEAS_MUX_ADC1] = 1u << ADC1D,
			[MEAS_MUX_ADC2] = 1u << ADC2D,
			[MEAS_MUX_ADC3] = 1u << ADC3D,
			[MEAS_MUX_ADC4] = 1u << ADC4D,
			[MEAS_MUX_ADC5] = 1u << ADC5D,
		};
		DIDR0 |= mux2didr[mux];
	}
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

void measure_adjust_set(enum measure_chan chan,
			fixpt_t adjustment)
{
#if CONF_ADJ
	meas.channels[chan].adjustment = adjustment;
#endif
}

fixpt_t measure_adjust_get(enum measure_chan chan)
{
#if CONF_ADJ
	return meas.channels[chan].adjustment;
#else
	return int_to_fixpt(0);
#endif
}

void measure_init(void)
{
	/* Discard the first measurement. */
	adc_trigger_chan(NULL);
	adc_busywait();
	(void)ADC;
	adc_disable();
}
