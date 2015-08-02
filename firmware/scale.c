/*
 * Xytronic LF-1600
 * Physical value scaling
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

#include "scale.h"


#ifndef SCALE_IMPL
# define SCALE_IMPL	1
#endif
#ifndef UNSCALE_IMPL
# define UNSCALE_IMPL	1
#endif


fixpt_t scale(int16_t raw, int16_t raw_lo, int16_t raw_hi,
	      fixpt_t phys_lo, fixpt_t phys_hi)
{
#if SCALE_IMPL == 1
	fixpt_big_t ret, a, b, c;
	int16_t tmp;

	/*         phys_lo * (raw_hi - raw) + phys_hi * (raw - raw_lo)
	 *  ret = -----------------------------------------------------
	 *                         raw_hi - raw_lo
	 */

	tmp = (int16_t)(raw_hi - raw);
	a = fixpt_big_mul(fixpt_inflate(phys_lo),
			  int_to_fixpt_big(tmp));

	tmp = (int16_t)(raw - raw_lo);
	b = fixpt_big_mul(fixpt_inflate(phys_hi),
			  int_to_fixpt_big(tmp));

	c = fixpt_big_add(a, b);

	tmp = (int16_t)(raw_hi - raw_lo);
	ret = fixpt_big_div(c,
			    int_to_fixpt_big(tmp));

	return fixpt_deflate(ret);
#elif SCALE_IMPL == 2
	fixpt_big_t a, b, c;
	int16_t tmp;

	/*         (phys_hi - phys_lo) * (raw - raw_lo)
	 *  ret = -------------------------------------- + phys_lo
	 *                  raw_hi - raw_lo
	 */

	a = fixpt_big_sub(fixpt_inflate(phys_hi),
			  fixpt_inflate(phys_lo));

	tmp = (int16_t)(raw - raw_lo);
	b = fixpt_big_mul(a, int_to_fixpt_big(tmp));

	tmp = (int16_t)(raw_hi - raw_lo);
	c = fixpt_big_div(b, int_to_fixpt_big(tmp));

	c = fixpt_big_add(c, phys_lo);

	return fixpt_deflate(c);
#else
# error
#endif
}

int16_t unscale(fixpt_t phys, fixpt_t phys_lo, fixpt_t phys_hi,
		int16_t raw_lo, int16_t raw_hi)
{
#if UNSCALE_IMPL == 1
	fixpt_big_t a, b, c;

	/*         raw_lo * (phys_hi - phys) + raw_hi * (phys - phys_lo)
	 *  ret = -----------------------------------------------------
	 *                         phys_hi - phys_lo
	 */

	a = fixpt_big_sub(fixpt_inflate(phys_hi),
			  fixpt_inflate(phys));
	a = fixpt_big_mul(a, int_to_fixpt_big(raw_lo));

	b = fixpt_big_sub(fixpt_inflate(phys),
			  fixpt_inflate(phys_lo));
	b = fixpt_big_mul(b, int_to_fixpt_big(raw_hi));

	c = fixpt_big_div(fixpt_big_add(a, b),
			  fixpt_big_sub(phys_hi, phys_lo));

	return (int16_t)fixpt_big_to_int(c);

#elif UNSCALE_IMPL == 2
	fixpt_big_t a, b, c;

	/*         (raw_hi - raw_lo) * (phys - phys_lo)
	 *  ret = -------------------------------------- + raw_lo
	 *                  phys_hi - phys_lo
	 */

	a = fixpt_big_mul(int_to_fixpt_big(raw_hi - raw_lo),
			  fixpt_big_sub(fixpt_inflate(phys),
					fixpt_inflate(phys_lo)));

	b = fixpt_big_sub(fixpt_inflate(phys_hi),
			  fixpt_inflate(phys_lo));

	c = fixpt_big_div(a, fixpt_deflate(b));

	c = fixpt_big_add(c, int_to_fixpt_big(raw_lo));

	return (int16_t)fixpt_to_int(fixpt_deflate(c));
#else
# error
#endif
}
