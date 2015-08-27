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

#include <avr/wdt.h>
#include <avr/eeprom.h>


/* Reboot the SOC. */
void reboot(void)
{
	/* Disable IRQs and use the watchdog
	 * to trigger a watchdog reset. */
	irq_disable();
	wdt_enable(WDTO_15MS);
	while (1);
	unreachable();
}

/* A fatal error occurred. */
void panic(void)
{
	//TODO: Try to get an error message out.
	reboot();
}

/* Read a block of data from the EEPROM.
 * This function resets the watchdog timer after each processed byte.
 */
void eeprom_read_block_wdtsafe(void *_dst, const void *_src, size_t n)
{
	uint8_t *dst = _dst;
	const uint8_t *src = _src;

	for ( ; n; n--, dst++, src++) {
		*dst = eeprom_read_byte(src);
		wdt_reset();
	}
}

/* Update a block of data in the EEPROM.
 * This function resets the watchdog timer after each processed byte.
 */
void eeprom_update_block_wdtsafe(const void *_src, void *_dst, size_t n)
{
	const uint8_t *src = _src;
	uint8_t *dst = _dst;

	for ( ; n; n--, dst++, src++) {
		eeprom_update_byte(dst, *src);
		wdt_reset();
	}
}
