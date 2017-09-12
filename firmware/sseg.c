/*
 * Xytronic LF-1600
 * Seven segment (plus decimal point) driver
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

#include "sseg.h"
#include "bitmap.h"


static const uint8_t __flash digit_to_segment_map[] = {
	['0' - '0']	= (1 << SSEG_A) | (1 << SSEG_B) | (1 << SSEG_C) |
			  (1 << SSEG_D) | (1 << SSEG_E) | (1 << SSEG_F) |
			  (0 << SSEG_G),
	['1' - '0']	= (0 << SSEG_A) | (1 << SSEG_B) | (1 << SSEG_C) |
			  (0 << SSEG_D) | (0 << SSEG_E) | (0 << SSEG_F) |
			  (0 << SSEG_G),
	['2' - '0']	= (1 << SSEG_A) | (1 << SSEG_B) | (0 << SSEG_C) |
			  (1 << SSEG_D) | (1 << SSEG_E) | (0 << SSEG_F) |
			  (1 << SSEG_G),
	['3' - '0']	= (1 << SSEG_A) | (1 << SSEG_B) | (1 << SSEG_C) |
			  (1 << SSEG_D) | (0 << SSEG_E) | (0 << SSEG_F) |
			  (1 << SSEG_G),
	['4' - '0']	= (0 << SSEG_A) | (1 << SSEG_B) | (1 << SSEG_C) |
			  (0 << SSEG_D) | (0 << SSEG_E) | (1 << SSEG_F) |
			  (1 << SSEG_G),
	['5' - '0']	= (1 << SSEG_A) | (0 << SSEG_B) | (1 << SSEG_C) |
			  (1 << SSEG_D) | (0 << SSEG_E) | (1 << SSEG_F) |
			  (1 << SSEG_G),
	['6' - '0']	= (1 << SSEG_A) | (0 << SSEG_B) | (1 << SSEG_C) |
			  (1 << SSEG_D) | (1 << SSEG_E) | (1 << SSEG_F) |
			  (1 << SSEG_G),
	['7' - '0']	= (1 << SSEG_A) | (1 << SSEG_B) | (1 << SSEG_C) |
			  (0 << SSEG_D) | (0 << SSEG_E) | (0 << SSEG_F) |
			  (0 << SSEG_G),
	['8' - '0']	= (1 << SSEG_A) | (1 << SSEG_B) | (1 << SSEG_C) |
			  (1 << SSEG_D) | (1 << SSEG_E) | (1 << SSEG_F) |
			  (1 << SSEG_G),
	['9' - '0']	= (1 << SSEG_A) | (1 << SSEG_B) | (1 << SSEG_C) |
			  (1 << SSEG_D) | (0 << SSEG_E) | (1 << SSEG_F) |
			  (1 << SSEG_G),
};

#define EMPTY_SEG	0

static const uint8_t __flash char_to_segment_map[] = {
	['A' - 'A']	= (1 << SSEG_A) | (1 << SSEG_B) | (1 << SSEG_C) |
			  (0 << SSEG_D) | (1 << SSEG_E) | (1 << SSEG_F) |
			  (1 << SSEG_G),
	['B' - 'A']	= (0 << SSEG_A) | (0 << SSEG_B) | (1 << SSEG_C) |
			  (1 << SSEG_D) | (1 << SSEG_E) | (1 << SSEG_F) |
			  (1 << SSEG_G),
	['C' - 'A']	= (1 << SSEG_A) | (0 << SSEG_B) | (0 << SSEG_C) |
			  (1 << SSEG_D) | (1 << SSEG_E) | (1 << SSEG_F) |
			  (0 << SSEG_G),
	['D' - 'A']	= (0 << SSEG_A) | (1 << SSEG_B) | (1 << SSEG_C) |
			  (1 << SSEG_D) | (1 << SSEG_E) | (0 << SSEG_F) |
			  (1 << SSEG_G),
	['E' - 'A']	= (1 << SSEG_A) | (0 << SSEG_B) | (0 << SSEG_C) |
			  (1 << SSEG_D) | (1 << SSEG_E) | (1 << SSEG_F) |
			  (1 << SSEG_G),
	['F' - 'A']	= (1 << SSEG_A) | (0 << SSEG_B) | (0 << SSEG_C) |
			  (0 << SSEG_D) | (1 << SSEG_E) | (1 << SSEG_F) |
			  (1 << SSEG_G),
	['G' - 'A']	= (1 << SSEG_A) | (0 << SSEG_B) | (1 << SSEG_C) |
			  (1 << SSEG_D) | (1 << SSEG_E) | (1 << SSEG_F) |
			  (1 << SSEG_G),
	['H' - 'A']	= (0 << SSEG_A) | (1 << SSEG_B) | (1 << SSEG_C) |
			  (0 << SSEG_D) | (1 << SSEG_E) | (1 << SSEG_F) |
			  (1 << SSEG_G),
	['I' - 'A']	= (0 << SSEG_A) | (0 << SSEG_B) | (0 << SSEG_C) |
			  (0 << SSEG_D) | (1 << SSEG_E) | (1 << SSEG_F) |
			  (0 << SSEG_G),
	['J' - 'A']	= (0 << SSEG_A) | (1 << SSEG_B) | (1 << SSEG_C) |
			  (1 << SSEG_D) | (0 << SSEG_E) | (0 << SSEG_F) |
			  (0 << SSEG_G),
	['K' - 'A']	= EMPTY_SEG,
	['L' - 'A']	= (0 << SSEG_A) | (0 << SSEG_B) | (0 << SSEG_C) |
			  (1 << SSEG_D) | (1 << SSEG_E) | (1 << SSEG_F) |
			  (0 << SSEG_G),
	['M' - 'A']	= EMPTY_SEG,
	['N' - 'A']	= (0 << SSEG_A) | (0 << SSEG_B) | (1 << SSEG_C) |
			  (0 << SSEG_D) | (1 << SSEG_E) | (0 << SSEG_F) |
			  (1 << SSEG_G),
	['O' - 'A']	= (1 << SSEG_A) | (1 << SSEG_B) | (1 << SSEG_C) |
			  (1 << SSEG_D) | (1 << SSEG_E) | (1 << SSEG_F) |
			  (0 << SSEG_G),
	['P' - 'A']	= (1 << SSEG_A) | (1 << SSEG_B) | (0 << SSEG_C) |
			  (0 << SSEG_D) | (1 << SSEG_E) | (1 << SSEG_F) |
			  (1 << SSEG_G),
	['Q' - 'A']	= EMPTY_SEG,
	['R' - 'A']	= (0 << SSEG_A) | (0 << SSEG_B) | (0 << SSEG_C) |
			  (0 << SSEG_D) | (1 << SSEG_E) | (0 << SSEG_F) |
			  (1 << SSEG_G),
};

static const uint8_t __flash segment_mask_minus =
			  (0 << SSEG_A) | (0 << SSEG_B) | (0 << SSEG_C) |
			  (0 << SSEG_D) | (0 << SSEG_E) | (0 << SSEG_F) |
			  (1 << SSEG_G);

static void mux_write(const struct sseg_digit_data *ddata,
		      bool enable)
{
	const struct sseg_iomap __flash *iomap = ddata->iomap;

	if (!iomap->mux_polarity)
		enable = !enable;

	if (enable)
		SFR_BYTE(iomap->mux_port) |= iomap->mux_mask;
	else
		SFR_BYTE(iomap->mux_port) &= (uint8_t)~iomap->mux_mask;
}

static void segment_write(const struct sseg_digit_data *ddata)
{
	const struct sseg_iomap __flash *iomap = ddata->iomap;
	const struct sseg_segment_iomap __flash *segment_iomap = iomap->segment;
	uint8_t enable_set, enable_mask;
	uint8_t value;

	enable_set = ddata->segment_enable_set;
	enable_mask = ddata->segment_enable_mask;
	if (!segment_iomap->segment_polarity)
		enable_set = (uint8_t)~enable_set;
	enable_set &= enable_mask;

	value = SFR_BYTE(segment_iomap->segment_port);
	value &= (uint8_t)~enable_mask;
	value |= enable_set;
	SFR_BYTE(segment_iomap->segment_port) = value;
}

void sseg_digit_set(struct sseg_digit_data *ddata,
		    char digit)
{
	const struct sseg_iomap __flash *iomap = ddata->iomap;
	const struct sseg_segment_iomap __flash *segment_iomap = iomap->segment;
	uint8_t segment_mask;
	uint8_t segment_enable_set;
	uint8_t dp;
	int8_t i;

	dp = (uint8_t)digit & (uint8_t)SSEG_DIGIT_DP;
	digit &= (char)~SSEG_DIGIT_DP;

	/* Get the generic segment mask for the digit. */
	if (digit >= 'a' && digit <= 'z')
		digit = (char)(digit - ('a' - 'A'));
	if (digit >= '0' && digit <= '9')
		segment_mask = digit_to_segment_map[digit - '0'];
	else if (digit >= 'A' &&
		 digit <= (char)('A' + ARRAY_SIZE(char_to_segment_map) - 1))
		segment_mask = char_to_segment_map[digit - 'A'];
	else if (digit == '-')
		segment_mask = segment_mask_minus;
	else
		segment_mask = 0;
	if (dp)
		segment_mask |= (1 << SSEG_DP);

	/* Get the digit specific mask from iomap. */
	segment_enable_set = 0;
	for (i = SSEG_A; i < SSEG_NR_SEGMENTS; i++) {
		if (segment_mask & BITMASK8(i))
			segment_enable_set |= segment_iomap->segment_masks[i];
	}

	ddata->segment_enable_set = segment_enable_set;
}

void sseg_multiplex(const struct sseg_digit_data *ddata_from,
		    const struct sseg_digit_data *ddata_to)
{
	/* Turn off the previous digit. */
	mux_write(ddata_from, 0);

	/* Write the new segment data. */
	segment_write(ddata_to);

	/* Turn on the new segment. */
	mux_write(ddata_to, 1);
}

void sseg_init(struct sseg_digit_data *ddata)
{
	const struct sseg_iomap __flash *iomap = ddata->iomap;
	const struct sseg_segment_iomap __flash *segment_iomap = iomap->segment;
	uint8_t segment_mask, mux_mask;
	int8_t i;

	/* Get the port masks. */
	segment_mask = 0;
	for (i = 0; i < SSEG_NR_SEGMENTS; i++)
		segment_mask |= segment_iomap->segment_masks[i];
	mux_mask = iomap->mux_mask;

	/* Configure the segment port. */
	ddata->segment_enable_mask = segment_mask;
	ddata->segment_enable_set = 0;
	segment_write(ddata);
	SFR_BYTE(segment_iomap->segment_ddr) |= segment_mask;

	/* Configure the multiplexer port. */
	mux_write(ddata, 0);
	SFR_BYTE(iomap->mux_ddr) |= mux_mask;
}

void sseg_exit(struct sseg_digit_data *ddata)
{
	const struct sseg_iomap __flash *iomap = ddata->iomap;
	const struct sseg_segment_iomap __flash *segment_iomap = iomap->segment;
	uint8_t segment_mask, mux_mask;

	mux_mask = iomap->mux_mask;
	segment_mask = ddata->segment_enable_mask;

	SFR_BYTE(segment_iomap->segment_port) &= (uint8_t)~segment_mask;
	SFR_BYTE(segment_iomap->segment_ddr) &= (uint8_t)~segment_mask;
	SFR_BYTE(iomap->mux_port) &= (uint8_t)~mux_mask;
	SFR_BYTE(iomap->mux_ddr) &= (uint8_t)~mux_mask;
}
