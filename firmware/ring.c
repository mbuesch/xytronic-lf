/*
 * Xytronic LF-1600
 * Ring buffer helpers
 *
 * Copyright (c) 2016 Michael Buesch <m@bues.ch>
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

#include "ring.h"


uint8_t ring_next(uint8_t current, uint8_t max_index)
{
	uint8_t next;

	next = (uint8_t)(current + 1u);
	if (next > max_index)
		next = 0u;

	return next;
}

uint8_t ring_prev(uint8_t current, uint8_t max_index)
{
	uint8_t prev;

	prev = (uint8_t)(current - 1u);
	if (prev > max_index)
		prev = max_index;

	return prev;
}
