/*
 * Xytronic LF-1600
 * Low pass filter
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

#include "filter.h"


uint16_t lp_filter_u16_run(struct lp_filter_u16 *lp, uint16_t in,
			   uint8_t filter_shift)
{
	uint24_t buf, out;

	buf = lp->filter_buf;
	buf -= buf >> filter_shift;
	buf += in;
	lp->filter_buf = buf;
	out = buf >> filter_shift;

	return (uint16_t)min(out, (uint16_t)UINT16_MAX);
}

fixpt_t lp_filter_fixpt_run(struct lp_filter_fixpt *lp, fixpt_t in,
			    fixpt_t div)
{
	fixpt_t buf, out;

	buf = lp->filter_buf;
	buf = fixpt_sub(buf, fixpt_div(buf, div));
	buf = fixpt_add(buf, in);
	lp->filter_buf = buf;
	out = fixpt_div(buf, div);

	return out;
}
