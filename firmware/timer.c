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


_timcnt_t _timer_count_now;


ISR(TIMER1_COMPA_vect)
{
	_timer_count_now++;
	mb();
}

#define CALCOCR(ps)	((uint32_t)F_CPU / ((uint32_t)(ps) * (uint32_t)TIMER_CPS))

#define CALCPS()	(((uint32_t)F_CPU / 1UL <= 0xFFFFUL) ? 1UL : \
			 (((uint32_t)F_CPU / 8UL <= 0xFFFFUL) ? 8UL : \
			  (((uint32_t)F_CPU / 64UL <= 0xFFFFUL) ? 64UL : \
			   (((uint32_t)F_CPU / 256UL <= 0xFFFFUL) ? 256UL : \
			    (((uint32_t)F_CPU / 1024UL <= 0xFFFFUL) ? 1024UL : \
			     0)))))

#define CALCCS(ps)	((ps) == 1UL ? ((0 << CS12) | (0 << CS11) | (1 << CS10)) : \
			 ((ps) == 8UL ? ((0 << CS12) | (1 << CS11) | (0 << CS10)) : \
			  ((ps) == 64UL ? ((0 << CS12) | (1 << CS11) | (1 << CS10)) : \
			   ((ps) == 256UL ? ((1 << CS12) | (0 << CS11) | (0 << CS10)) : \
			    ((ps) == 1024UL ? ((1 << CS12) | (0 << CS11) | (1 << CS10)) : \
			     0)))))

void timer_init(void)
{
	const uint16_t ps = CALCPS();

	/* Initialize timer-1 as system timer. */
	OCR1A = CALCOCR(ps);
	TCNT1 = 0;
	TCCR1A = 0;
	/* CTC mode */
	TCCR1B = (1 << WGM12) | (uint8_t)(CALCCS(ps));
#ifdef TCCR1C
	TCCR1C = 0;
#endif
	/* Enable OC interrupt. */
#ifdef TIMSK1
	TIFR1 = (1 << OCF1A);
	TIMSK1 |= (1 << OCIE1A);
#else
	TIFR = (1 << OCF1A);
	TIMSK |= (1 << OCIE1A);
#endif
}
