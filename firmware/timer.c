/*
 * Timer routines
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

#include "timer.h"

#include <avr/io.h>


static _timcnt_t _timer_count_now;


_timcnt_t _timer_get_now(void)
{
	_timcnt_t count;
	uint8_t sreg;

	sreg = irq_disable_save();
	count = _timer_count_now;
	irq_restore(sreg);

	return count;
}

ISR(TIMER0_COMPA_vect)
{
	_timer_count_now++;
	mb();
}

bool timer_expired(const struct timer *timer)
{
	return _timer_count_since(timer) >= 0;
}

void timer_init(void)
{
	build_assert(F_CPU == 8000000ul);

	/* Initialize timer-0 as system timer. */
	OCR0A = 125;
	TCNT0 = 0;
	/* CTC mode; PS 64 */
	TCCR0A = (1 << WGM01) | (0 << WGM00);
	TCCR0B = (0 << WGM02) | (0 << CS02) | (1 << CS01) | (1 << CS00);
	/* Enable OC interrupt. */
	TIFR0 = (1 << OCF0A);
	TIMSK0 |= (1 << OCIE0A);
}
