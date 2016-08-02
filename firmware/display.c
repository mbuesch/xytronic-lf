/*
 * Xytronic LF-1600
 * Display routines
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

#include "display.h"
#include "sseg.h"
#include "timer.h"
#include "bitmap.h"
#include "ring.h"

#include <string.h>


#define DISPLAY_MUX_PERIOD_MS	1


static const struct sseg_segment_iomap __flash big_digit = {
	.segment_ddr		= SFR_ADDR(DDRD),
	.segment_port		= SFR_ADDR(PORTD),
	.segment_masks		= {
		[SSEG_A]	= 1u << PD7,
		[SSEG_B]	= 1u << PD0,
		[SSEG_C]	= 1u << PD2,
		[SSEG_D]	= 1u << PD4,
		[SSEG_E]	= 1u << PD5,
		[SSEG_F]	= 1u << PD6,
		[SSEG_G]	= 1u << PD1,
		[SSEG_DP]	= 1u << PD3,
	},
	.segment_polarity	= 0,
};

static const struct sseg_segment_iomap __flash small_digit = {
	.segment_ddr		= SFR_ADDR(DDRD),
	.segment_port		= SFR_ADDR(PORTD),
	.segment_masks		= {
		[SSEG_A]	= 1u << PD2,
		[SSEG_B]	= 1u << PD3,
		[SSEG_C]	= 1u << PD0,
		[SSEG_D]	= 1u << PD1,
		[SSEG_E]	= 1u << PD6,
		[SSEG_F]	= 1u << PD4,
		[SSEG_G]	= 1u << PD7,
		[SSEG_DP]	= 1u << PD5,
	},
	.segment_polarity	= 0,
};

static const struct sseg_iomap __flash digit_iomaps[] = {
	{
		.segment		= &big_digit,
		.mux_ddr		= SFR_ADDR(DDRC),
		.mux_port		= SFR_ADDR(PORTC),
		.mux_mask		= 1u << PC3,
		.mux_polarity		= 1,
	}, {
		.segment		= &big_digit,
		.mux_ddr		= SFR_ADDR(DDRC),
		.mux_port		= SFR_ADDR(PORTC),
		.mux_mask		= 1u << PC4,
		.mux_polarity		= 1,
	}, {
		.segment		= &big_digit,
		.mux_ddr		= SFR_ADDR(DDRC),
		.mux_port		= SFR_ADDR(PORTC),
		.mux_mask		= 1u << PC5,
		.mux_polarity		= 1,
	}, {
		.segment		= &small_digit,
		.mux_ddr		= SFR_ADDR(DDRB),
		.mux_port		= SFR_ADDR(PORTB),
		.mux_mask		= 1u << PB7,
		.mux_polarity		= 1,
	},
};

#define DISPLAY_NR_DIGITS	ARRAY_SIZE(digit_iomaps)

struct display_digit {
	struct sseg_digit_data sseg;
	bool dp_force_enable;
	bool dp_on;
};

struct display_context {
	bool enabled;

	struct display_digit digit_data[DISPLAY_NR_DIGITS];
	uint8_t digit_mux_count;

	struct timer mux_timer;
};

static struct display_context display;


void display_force_dp(int8_t dp, bool force_en, bool dp_on)
{
	display.digit_data[dp].dp_force_enable = force_en;
	display.digit_data[dp].dp_on = dp_on;
}

void display_show(const char *digits)
{
	struct display_digit *digit_data;
	char c;

	digit_data = &display.digit_data[0];
	do {
		c = digits[0];
		if (c == '\0') {
			c = ' ';
		} else {
			if (digits[1] == '.') {
				c = (char)(c | SSEG_DIGIT_DP);
				digits++;
			}
			if (digit_data->dp_force_enable) {
				if (digit_data->dp_on)
					c = (char)(c | SSEG_DIGIT_DP);
				else
					c = (char)(c & (char)~SSEG_DIGIT_DP);
			}
			digits++;
		}
		sseg_digit_set(&digit_data->sseg, c);

		digit_data++;
	} while (digit_data < &display.digit_data[DISPLAY_NR_DIGITS]);
}

void display_work(void)
{
	uint8_t cur_mux, next_mux;

	if (!display.enabled)
		return;

	if (!timer_expired(&display.mux_timer))
		return;
	timer_add(&display.mux_timer, DISPLAY_MUX_PERIOD_MS);

	cur_mux = display.digit_mux_count;
	next_mux = ring_next(cur_mux, DISPLAY_NR_DIGITS - 1u);
	display.digit_mux_count = next_mux;

	sseg_multiplex(&display.digit_data[cur_mux].sseg,
		       &display.digit_data[next_mux].sseg);
}

void display_enable(bool enable)
{
	uint8_t i;
	struct sseg_digit_data *ddata;

	if (enable == display.enabled)
		return;

	display.enabled = enable;
	if (enable) {
		for (i = 0; i < DISPLAY_NR_DIGITS; i++) {
			ddata = &display.digit_data[i].sseg;

			ddata->iomap = &digit_iomaps[i];
			sseg_init(ddata);
		}
		timer_set_now(&display.mux_timer);
	} else {
		for (i = 0; i < DISPLAY_NR_DIGITS; i++) {
			ddata = &display.digit_data[i].sseg;

			sseg_exit(ddata);
		}
	}
}

void display_init(void)
{
	display_enable(true);
}
