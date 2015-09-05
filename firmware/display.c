/*
 * Xytronic LF-1600
 * Display routines
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

#include "display.h"
#include "sseg.h"
#include "timer.h"
#include "bitmap.h"

#include <string.h>


#define DISPLAY_MUX_PERIOD_MS	1


static const struct sseg_iomap __flash digit_iomaps[] = {
	{
		.segment_ddr	= SFR_ADDR(DDRD),
		.segment_port	= SFR_ADDR(PORTD),
		.segment_masks	= {
			[SSEG_A]	= 1u << PD7,
			[SSEG_B]	= 1u << PD0,
			[SSEG_C]	= 1u << PD2,
			[SSEG_D]	= 1u << PD4,
			[SSEG_E]	= 1u << PD5,
			[SSEG_F]	= 1u << PD6,
			[SSEG_G]	= 1u << PD1,
			[SSEG_DP]	= 1u << PD3,
		},
		.mux_ddr	= SFR_ADDR(DDRC),
		.mux_port	= SFR_ADDR(PORTC),
		.mux_mask	= 1u << PC3,
		.mux_polarity	= 1,
	}, {
		.segment_ddr	= SFR_ADDR(DDRD),
		.segment_port	= SFR_ADDR(PORTD),
		.segment_masks	= {
			[SSEG_A]	= 1u << PD7,
			[SSEG_B]	= 1u << PD0,
			[SSEG_C]	= 1u << PD2,
			[SSEG_D]	= 1u << PD4,
			[SSEG_E]	= 1u << PD5,
			[SSEG_F]	= 1u << PD6,
			[SSEG_G]	= 1u << PD1,
			[SSEG_DP]	= 1u << PD3,
		},
		.mux_ddr	= SFR_ADDR(DDRC),
		.mux_port	= SFR_ADDR(PORTC),
		.mux_mask	= 1u << PC4,
		.mux_polarity	= 1,
	}, {
		.segment_ddr	= SFR_ADDR(DDRD),
		.segment_port	= SFR_ADDR(PORTD),
		.segment_masks	= {
			[SSEG_A]	= 1u << PD7,
			[SSEG_B]	= 1u << PD0,
			[SSEG_C]	= 1u << PD2,
			[SSEG_D]	= 1u << PD4,
			[SSEG_E]	= 1u << PD5,
			[SSEG_F]	= 1u << PD6,
			[SSEG_G]	= 1u << PD1,
			[SSEG_DP]	= 1u << PD3,
		},
		.mux_ddr	= SFR_ADDR(DDRC),
		.mux_port	= SFR_ADDR(PORTC),
		.mux_mask	= 1u << PC5,
		.mux_polarity	= 1,
	}, {
		.segment_ddr	= SFR_ADDR(DDRD),
		.segment_port	= SFR_ADDR(PORTD),
		.segment_masks	= {
			[SSEG_A]	= 1u << PD2,
			[SSEG_B]	= 1u << PD3,
			[SSEG_C]	= 1u << PD0,
			[SSEG_D]	= 1u << PD1,
			[SSEG_E]	= 1u << PD6,
			[SSEG_F]	= 1u << PD4,
			[SSEG_G]	= 1u << PD7,
			[SSEG_DP]	= 1u << PD5,
		},
		.mux_ddr	= SFR_ADDR(DDRB),
		.mux_port	= SFR_ADDR(PORTB),
		.mux_mask	= 1u << PB7,
		.mux_polarity	= 1,
	},
};

#define DISPLAY_NR_DIGITS	ARRAY_SIZE(digit_iomaps)

struct display_context {
	bool enabled;

	struct sseg_digit_data digit_data[DISPLAY_NR_DIGITS];

	uint8_t dp_force_enable;
	uint8_t dp_force_mask;

	uint8_t digit_mux_count;

	struct timer mux_timer;
};

static struct display_context display;


void display_force_dp(int8_t dp, bool force, bool enable)
{
	uint8_t mask;

	mask = BITMASK8(dp);
	if (force)
		display.dp_force_enable |= mask;
	else
		display.dp_force_enable &= (uint8_t)~mask;
	if (enable)
		display.dp_force_mask |= mask;
	else
		display.dp_force_mask &= (uint8_t)~mask;
}

void display_show(const char *digits)
{
	char c;
	uint8_t i = 0;
	uint8_t mask;

	while (1) {
		c = digits[0];
		if (c == '\0')
			break;

		if (digits[1] == '.' || digits[1] == ',') {
			c = (char)(c | SSEG_DIGIT_DP);
			digits++;
		}
		mask = BITMASK8(i);
		if (display.dp_force_enable & mask) {
			if (display.dp_force_mask & mask)
				c = (char)(c | SSEG_DIGIT_DP);
			else
				c = (char)(c & (char)~SSEG_DIGIT_DP);
		}
		sseg_digit_set(&display.digit_data[i], c);

		i++;
		digits++;
	}
	for ( ; i < DISPLAY_NR_DIGITS; i++)
		sseg_digit_set(&display.digit_data[i], ' ');
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
	next_mux = (uint8_t)(cur_mux + 1u);
	if (next_mux >= DISPLAY_NR_DIGITS)
		next_mux = 0;
	display.digit_mux_count = next_mux;

	sseg_multiplex(&display.digit_data[cur_mux],
		       &display.digit_data[next_mux]);
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
			ddata = &(display.digit_data[i]);

			ddata->iomap = &digit_iomaps[i];
			sseg_init(ddata);
		}
		timer_arm(&display.mux_timer, 0);
	} else {
		for (i = 0; i < DISPLAY_NR_DIGITS; i++) {
			ddata = &(display.digit_data[i]);

			sseg_exit(ddata);
		}
	}
}

void display_init(void)
{
	memset(&display, 0, sizeof(display));
	display_enable(true);
}
