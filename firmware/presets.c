/*
 * Xytronic LF-1600
 * Temperature presets
 *
 * Copyright (c) 2016-2017 Michael Buesch <m@bues.ch>
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

#include "presets.h"
#include "settings.h"
#include "controller_temp.h"
#include "ring.h"
#include "menu.h"

#include <string.h>


#if CONF_PRESETS
static void presets_store(void)
{
	store_settings();
	contrtemp_update_setpoint();
	menu_request_display_update();
}
#endif

void presets_next(void)
{
#if CONF_PRESETS
	struct settings *settings;

	settings = get_settings();
	settings->temp_setpoint_active = ring_next(settings->temp_setpoint_active,
						   NR_PRESETS - 1u);
	presets_store();
#endif
}

void presets_prev(void)
{
#if CONF_PRESETS
	struct settings *settings;

	settings = get_settings();
	settings->temp_setpoint_active = ring_prev(settings->temp_setpoint_active,
						   NR_PRESETS - 1u);
	presets_store();
#endif
}

uint8_t presets_get_active_index(void)
{
#if CONF_PRESETS
	return get_settings()->temp_setpoint_active;
#else
	return 0u;
#endif
}

fixpt_t presets_get_active_value(void)
{
	struct settings *settings;
	uint8_t active_index;
	fixpt_t value;

	settings = get_settings();
	active_index = settings->temp_setpoint_active;
	value = settings->temp_setpoint[active_index];

	return value;
}

void presets_set_active_value(fixpt_t value)
{
	struct settings *settings;
	uint8_t active_index;

	settings = get_settings();
	active_index = settings->temp_setpoint_active;
	settings->temp_setpoint[active_index] = value;

	presets_store();
}

void presets_init(void)
{
}
