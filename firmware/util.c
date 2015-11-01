/*
 * Utility functions
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

#include "util.h"


int32_t pow_int(int16_t b, uint8_t e)
{
	int32_t x, y;

	if (!e)
		return 1;

	x = b;
	while (!(e & 1)) {
		e >>= 1;
		x *= x;
	}
	y = x;
	e >>= 1;
	while (e) {
		x *= x;
		if (e & 1)
			y *= x;
		e >>= 1;
	}

	return y;
}
