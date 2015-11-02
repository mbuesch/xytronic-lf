/*
 * Xytronic LF-1600
 * Fixed point data type
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

#include "fixpt.h"


fixpt_t fixpt_deflate(fixpt_big_t a)
{
	if (a >= FIXPT_MAX)
		return FIXPT_MAX;
	if (a <= FIXPT_MIN)
		return FIXPT_MIN;

	return (fixpt_t)a;
}

int32_t fixpt_big_to_int(fixpt_big_t p)
{
	int32_t i;

	if (p < 0) {
		p = -p;
		i = ((int32_t)p + (int32_t)(1L << (FIXPT_SHIFT - 1))) >> FIXPT_SHIFT;
		i = -i;
	} else {
		i = ((int32_t)p + (int32_t)(1L << (FIXPT_SHIFT - 1))) >> FIXPT_SHIFT;
	}

	return i;
}

int32_t fixpt_get_int_part(fixpt_t p)
{
	if (p >= int_to_fixpt(0))
		return (int32_t)p >> FIXPT_SHIFT;
	else
		return -((-(int32_t)p) >> FIXPT_SHIFT);
}

uint32_t fixpt_get_dec_fract(fixpt_t p, uint8_t nr_digits)
{
	uint32_t dec_mask;
	uint32_t dec_fract;

	/* Mask the fractional part. */
	p = fixpt_abs(p) & ((1L << FIXPT_SHIFT) - 1);
	/* Calculate the decimal digit mask. */
	dec_mask = (uint32_t)pow_int(10, nr_digits);
	/* Calculate the decimal fractional. */
	dec_fract = (uint32_t)udiv_round((uint32_t)p * dec_mask,
					 (uint32_t)(1UL << FIXPT_SHIFT));

	return dec_fract;
}

fixpt_big_t fixpt_big_mul(fixpt_big_t a, fixpt_big_t b)
{
	fixpt_big_t tmp;

	/* Multiply */
	tmp = a * b;
	/* Round */
	tmp += (fixpt_big_t)1L << (FIXPT_SHIFT - 1);
	/* Scale */
	tmp >>= FIXPT_SHIFT;

	return tmp;
}

fixpt_big_t fixpt_big_div(fixpt_big_t a, fixpt_big_t b)
{
	fixpt_big_t tmp;

	/* Scale */
	tmp = a << FIXPT_SHIFT;
	/* Round */
	if ((tmp >= 0 && b >= 0) || (tmp < 0 && b < 0))
		tmp += b / 2;
	else
		tmp -= b / 2;
	/* Divide */
	tmp /= b;

	return tmp;
}

fixpt_big_t fixpt_big_mul_div(fixpt_big_t a, fixpt_big_t b, fixpt_big_t c)
{
	fixpt_big_t tmp;

	tmp = fixpt_big_mul(a, b);
	tmp = fixpt_big_div(tmp, c);

	return tmp;
}

fixpt_t fixpt_abs(fixpt_t a)
{
	if (a < int_to_fixpt(0))
		return fixpt_neg(a);
	return a;
}

fixpt_t fixpt_add_limited(fixpt_t a, fixpt_t b,
			  fixpt_t lo_lim, fixpt_t hi_lim)
{
	fixpt_t ret;

	ret = fixpt_add(a, b);
	if (ret > hi_lim)
		ret = hi_lim;
	if (ret < lo_lim)
		ret = lo_lim;

	return ret;
}
