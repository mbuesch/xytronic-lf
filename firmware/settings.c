/*
 * Xytronic LF-1600
 * Settings handling
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

#include "settings.h"
#include "util.h"
#include "timer.h"

#include <string.h>

#include <avr/eeprom.h>


static struct timer settings_timer;
static bool settings_store_request;
static uint8_t settings_ee_index;

static struct settings settings_cache;


/* The permanent EEPROM storage. */
#define NR_EE_SETTINGS	((E2END + 1) / sizeof(struct settings))
static struct settings EEMEM ee_settings[NR_EE_SETTINGS] = {
	[0 ... NR_EE_SETTINGS - 1] = {
		.temp_setpoint	= FLOAT_TO_FIXPT(330.0),
		.serial		= 0,
	},
};


struct settings * get_settings(void)
{
	return &settings_cache;
}

void store_settings(void)
{
	settings_store_request = true;
	timer_arm(&settings_timer, 1000);
}

void settings_work(void)
{
	uint8_t index;

	if (!settings_store_request)
		return;
	if (!timer_expired(&settings_timer))
		return;
	settings_store_request = false;

	/* Increment the serial number. This might wrap. */
	settings_cache.serial = (uint8_t)(settings_cache.serial + 1u);

	/* Get the store index. */
	index = settings_ee_index;
	index = (uint8_t)(index + 1u);
	if (index >= ARRAY_SIZE(ee_settings))
		index = 0;
	settings_ee_index = index;

	/* Store the settings in eeprom. */
	eeprom_update_block_wdtsafe(&settings_cache,
				    &ee_settings[index],
				    sizeof(settings_cache));
}

void settings_init(void)
{
	uint8_t next_index, found_index;
	uint8_t serial, next_serial;

	/* Find the latest settings in the eeprom.
	 * The latest setting is the one with the largest
	 * index. However, wrap around must be considered. */
	serial = eeprom_read_byte(&ee_settings[0].serial);
	next_index = 0;
	do {
		found_index = next_index;

		next_index = (uint8_t)(next_index + 1u);
		if (next_index >= ARRAY_SIZE(ee_settings))
			next_index = 0u;

		next_serial = eeprom_read_byte(&ee_settings[next_index].serial);
		if (next_serial != ((serial + 1u) & 0xFFu))
			break;

		serial = next_serial;
	} while (next_index != 0);
	settings_ee_index = found_index;

	/* Read it from eeprom. */
	eeprom_read_block_wdtsafe(&settings_cache,
				  &ee_settings[found_index],
				  sizeof(settings_cache));
}
