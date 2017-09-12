/*
 * Xytronic LF-1600
 * Button routines
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

#include "buttons.h"
#include "timer.h"

#include <string.h>


#define BUTTONS_DDR		DDRB
#define BUTTONS_PORT		PORTB
#define BUTTONS_PIN		PINB

#define BUTTON_BIT_IRON		PB0
#define BUTTON_BIT_SET		PB2
#define BUTTON_BIT_MINUS	PB3
#define BUTTON_BIT_PLUS		PB4

#define BUTTONS_MASK		((uint8_t)((1 << BUTTON_BIT_IRON) |\
					   (1 << BUTTON_BIT_SET) |\
					   (1 << BUTTON_BIT_MINUS) |\
					   (1 << BUTTON_BIT_PLUS)))

#define BUTTONS_DEBOUNCE_MS	10
#define BUTTONS_ACTIVE_LOW	1


struct buttons_context {
	uint8_t prev_state;
	struct timer debounce_timer;
};

static struct buttons_context buttons;

static const uint8_t __flash button_id_to_port_mask[] = {
	[BUTTON_SET]	= 1u << BUTTON_BIT_SET,
	[BUTTON_MINUS]	= 1u << BUTTON_BIT_MINUS,
	[BUTTON_PLUS]	= 1u << BUTTON_BIT_PLUS,
	[BUTTON_IRON]	= 1u << BUTTON_BIT_IRON,
};


static uint8_t buttons_get_raw(void)
{
	uint8_t state;

	mb();
	state = BUTTONS_PIN;
	if (BUTTONS_ACTIVE_LOW)
		state = (uint8_t)~state;
	state = (uint8_t)(state & BUTTONS_MASK);

	return state;
}

uint8_t button_is_pressed(enum button_id button)
{
	uint8_t state, mask;

	mask = button_id_to_port_mask[button];
	state = buttons_get_raw();

	return state & mask;
}

void contrtemp_button_handler(enum button_id button,
			      enum button_state bstate);
void menu_button_handler(enum button_id button,
			 enum button_state bstate);

static void call_button_handlers(enum button_id id, enum button_state bstate)
{
	contrtemp_button_handler(id, bstate);
	menu_button_handler(id, bstate);
}

void buttons_work(void)
{
	uint8_t state, pos_edge, neg_edge, mask;
	int8_t i;
	enum button_id id;

	if (!timer_expired(&buttons.debounce_timer))
		return;
	timer_add(&buttons.debounce_timer, BUTTONS_DEBOUNCE_MS);

	state = buttons_get_raw();
	pos_edge = (state & ~buttons.prev_state) & BUTTONS_MASK;
	neg_edge = (~state & buttons.prev_state) & BUTTONS_MASK;
	buttons.prev_state = state;

	for (i = 0; i < NR_BUTTONS; i++) {
		id = (enum button_id)i;
		mask = button_id_to_port_mask[i];

		if (pos_edge & mask)
			call_button_handlers(id, BSTATE_POSEDGE);
		else if (neg_edge & mask)
			call_button_handlers(id, BSTATE_NEGEDGE);

		if (state & mask)
			call_button_handlers(id, BSTATE_PRESSED);
	}
}

void buttons_init(void)
{
	BUTTONS_DDR &= (uint8_t)~BUTTONS_MASK;
	BUTTONS_PORT |= BUTTONS_MASK;
	_delay_ms(50);

	timer_set_now(&buttons.debounce_timer);
}
