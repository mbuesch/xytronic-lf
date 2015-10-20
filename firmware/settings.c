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
#include "controller_temp.h"

#include <string.h>

#include <avr/eeprom.h>


struct settings_context {
	struct timer store_timer;
	bool store_request;

	struct settings cache;

	uint8_t ee_index;
	uint8_t ee_write_offset;
};


static struct settings_context settings;

/* The permanent EEPROM storage. */
#define NR_EE_SETTINGS	((E2END + 1) / sizeof(struct settings))
static struct settings EEMEM ee_settings[NR_EE_SETTINGS] = {
	[0 ... NR_EE_SETTINGS - 1] = {
		.temp_setpoint	= FLOAT_TO_FIXPT(330.0),
		.temp_k[TEMPBOOST_NORMAL] = {
			.kp		= FLOAT_TO_FIXPT(CONTRTEMP_PID_KP_NORMAL),
			.ki		= FLOAT_TO_FIXPT(CONTRTEMP_PID_KI_NORMAL),
			.kd		= FLOAT_TO_FIXPT(CONTRTEMP_PID_KD_NORMAL),
			.d_decay_div	= FLOAT_TO_FIXPT(CONTRTEMP_PID_D_DECAY_NORMAL),
		},
		.temp_k[TEMPBOOST_BOOST1] = {
			.kp		= FLOAT_TO_FIXPT(CONTRTEMP_PID_KP_BOOST1),
			.ki		= FLOAT_TO_FIXPT(CONTRTEMP_PID_KI_BOOST1),
			.kd		= FLOAT_TO_FIXPT(CONTRTEMP_PID_KD_BOOST1),
			.d_decay_div	= FLOAT_TO_FIXPT(CONTRTEMP_PID_D_DECAY_BOOST1),
		},
		.temp_k[TEMPBOOST_BOOST2] = {
			.kp		= FLOAT_TO_FIXPT(CONTRTEMP_PID_KP_BOOST2),
			.ki		= FLOAT_TO_FIXPT(CONTRTEMP_PID_KI_BOOST2),
			.kd		= FLOAT_TO_FIXPT(CONTRTEMP_PID_KD_BOOST2),
			.d_decay_div	= FLOAT_TO_FIXPT(CONTRTEMP_PID_D_DECAY_BOOST2),
		},
		.serial		= 0,
	},
};


static uint8_t ee_read_byte(uint16_t addr)
{
	uint8_t sreg, data;

	sreg = irq_disable_save();

	eeprom_busy_wait();
	EEAR = addr;
	EECR |= (1 << EERE);
	data = EEDR;

	irq_restore(sreg);

	return data;
}

static void ee_read_block(void *dest, uint16_t addr, uint8_t count)
{
	uint8_t *d = dest;

	for ( ; count; count--, d++, addr++)
		*d = ee_read_byte(addr);
}

ISR(EE_READY_vect)
{
	uint16_t address;
	uint8_t data;
	uint8_t index, offset;

	index = settings.ee_index;
	offset = settings.ee_write_offset;

	address = (uint16_t)((uint8_t *)&ee_settings[index] + offset);
	data = *((uint8_t *)&settings.cache + offset);

	EEAR = address;
	/* Read the byte. */
	EECR |= (1 << EERE);
	if (EEDR == data) {
		/* The data in EEPROM matches the data to be written.
		 * No write is needed.
		 * This interrupt will trigger again immediately.
		 */
	} else {
		/* Start programming of the byte.
		 * This interrupt will trigger again when programming finished.
		 */
		EEDR = data;
		EECR |= (1 << EEMPE);
		EECR |= (1 << EEPE);
	}

	offset++;
	settings.ee_write_offset = offset;
	if (offset >= sizeof(struct settings)) {
		/* Done writing. Disable the interrupt. */
		EECR &= (uint8_t)~(1 << EERIE);
	}
}

struct settings * get_settings(void)
{
	return &settings.cache;
}

void store_settings(void)
{
	settings.store_request = true;
	timer_arm(&settings.store_timer, 1000);
}

void settings_work(void)
{
	uint8_t sreg, index;

	if (!settings.store_request)
		return;
	if (!timer_expired(&settings.store_timer))
		return;
	settings.store_request = false;

	/* Increment the serial number. This might wrap. */
	settings.cache.serial++;

	sreg = irq_disable_save();

	/* Increment the store index. */
	index = settings.ee_index;
	index++;
	if (index >= ARRAY_SIZE(ee_settings))
		index = 0;
	settings.ee_index = index;

	/* Reset the store byte offset. */
	settings.ee_write_offset = 0;

	/* Enable the eeprom-ready interrupt.
	 * It will fire, if the EEPROM is ready.
	 */
	EECR |= (1 << EERIE);

	irq_restore(sreg);
}

void settings_init(void)
{
	uint8_t next_index, found_index;
	uint8_t serial, next_serial;

	build_assert(sizeof(struct settings) == 64);

	memset(&settings, 0, sizeof(settings));

	/* Find the latest settings in the eeprom.
	 * The latest setting is the one with the largest
	 * index. However, wrap around must be considered.
	 */
	serial = ee_read_byte((uint16_t)&ee_settings[0].serial);
	next_index = 0;
	do {
		found_index = next_index;

		next_index++;
		if (next_index >= ARRAY_SIZE(ee_settings))
			next_index = 0u;

		next_serial = ee_read_byte((uint16_t)&ee_settings[next_index].serial);
		if (next_serial != ((serial + 1u) & 0xFFu))
			break;

		serial = next_serial;
	} while (next_index != 0);
	settings.ee_index = found_index;

	/* Read settings from EEPROM. */
	ee_read_block(&settings.cache,
		      (uint16_t)&ee_settings[found_index],
		      sizeof(settings.cache));
}
