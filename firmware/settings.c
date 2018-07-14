/*
 * Xytronic LF-1600
 * Settings handling
 *
 * Copyright (c) 2015-2017 Michael Buesch <m@bues.ch>
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
#include "presets.h"
#include "ring.h"

#include <string.h>

#include <avr/eeprom.h>


struct settings_context {
	struct timer store_timer;
	bool store_request;

	struct settings cache;

#if CONF_EERING
	uint8_t ee_index;
#endif
	uint8_t ee_write_offset;
};


static struct settings_context settings;

#if CONF_EERING
# define NR_EE_SETTINGS		((E2END + 1) / sizeof(struct settings))
#else
# define NR_EE_SETTINGS		1
#endif

/* The permanent EEPROM storage. */
static struct settings EEMEM ee_settings[NR_EE_SETTINGS] = {
	[0 ... NR_EE_SETTINGS - 1] = {
		.temp_k[TEMPBOOST_NORMAL] = {
			.kp		= FLOAT_TO_FIXPT(CONTRTEMP_PID_KP_NORMAL),
			.ki		= FLOAT_TO_FIXPT(CONTRTEMP_PID_KI_NORMAL),
			.kd		= FLOAT_TO_FIXPT(CONTRTEMP_PID_KD_NORMAL),
			.d_decay_div	= FLOAT_TO_FIXPT(CONTRTEMP_PID_D_DECAY_NORMAL),
		},
#if CONF_BOOST
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
#endif
		.temp_setpoint[0]	= FLOAT_TO_FIXPT(PRESET_DEFAULT0),
#if CONF_PRESETS
		.temp_setpoint[1]	= FLOAT_TO_FIXPT(PRESET_DEFAULT1),
		.temp_setpoint[2]	= FLOAT_TO_FIXPT(PRESET_DEFAULT2),
		.temp_setpoint[3]	= FLOAT_TO_FIXPT(PRESET_DEFAULT3),
		.temp_setpoint[4]	= FLOAT_TO_FIXPT(PRESET_DEFAULT4),
		.temp_setpoint[5]	= FLOAT_TO_FIXPT(PRESET_DEFAULT5),
#endif
		.temp_setpoint_active	= PRESET_DEFAULT_INDEX,
#if CONF_IDLE
		.temp_idle_setpoint	= FLOAT_TO_FIXPT(CONTRTEMP_DEF_IDLE_SETPOINT),
#endif
		.serial		= 0,
	},
};


static uint8_t ee_read_byte(uint16_t addr)
{
	uint8_t data;

	eeprom_busy_wait();
	EEAR = addr;
	EECR |= (1 << EERE);
	data = EEDR;

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
	uint8_t offset;
	uint8_t index = 0u;

#if CONF_EERING
	index = settings.ee_index;
#endif
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

	settings.ee_write_offset = ++offset;
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
	if (!settings.store_request)
		return;
	if (!timer_expired(&settings.store_timer))
		return;

	irq_disable();

	settings.store_request = false;

#if CONF_EERING
	/* Increment the serial number. This might wrap. */
	settings.cache.serial++;

	/* Increment the store index. */
	settings.ee_index = ring_next(settings.ee_index,
				      ARRAY_SIZE(ee_settings) - 1u);
#endif

	/* Reset the store byte offset. */
	settings.ee_write_offset = 0;

	/* Enable the eeprom-ready interrupt.
	 * It will fire, if the EEPROM is ready.
	 */
	EECR |= (1 << EERIE);

	irq_enable();
}

void settings_init(void)
{
#if CONF_EERING
	uint8_t next_index, serial, next_serial;
#endif
	uint8_t found_index = 0u;

#if !defined(__CHECKER__) && !defined(SIMULATOR)
	build_assert(sizeof(struct settings) == 64);
#endif
	build_assert(SEA_SIZE(struct settings, temp_k) >= NR_BOOST_MODES);
	build_assert(SEA_SIZE(struct settings, temp_setpoint) >= NR_PRESETS);

#if CONF_EERING
	/* Find the latest settings in the eeprom.
	 * The latest setting is the one with the largest
	 * index. However, wrap around must be considered.
	 */
	serial = ee_read_byte((uint16_t)&ee_settings[0].serial);
	next_index = 0;
	do {
		found_index = next_index;

		next_index = ring_next(next_index, ARRAY_SIZE(ee_settings) + 1u);

		next_serial = ee_read_byte((uint16_t)&ee_settings[next_index].serial);
		if (next_serial != ((serial + 1u) & 0xFFu))
			break;

		serial = next_serial;
	} while (next_index != 0);
	settings.ee_index = found_index;
#endif /* CONF_EERING */

	/* Read settings from EEPROM. */
	ee_read_block(&settings.cache,
		      (uint16_t)&ee_settings[found_index],
		      sizeof(settings.cache));
}
