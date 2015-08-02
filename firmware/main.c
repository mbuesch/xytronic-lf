/*
 * Xytronic LF-1600
 * Open Source firmware
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

#include "main.h"
#include "measure.h"
#include "measure_current.h"
#include "measure_temp.h"
#include "controller_current.h"
#include "timer.h"
#include "util.h"

#include <avr/io.h>
#include <avr/wdt.h>


int main(void) _mainfunc;
int main(void)
{
	irq_disable();
	wdt_enable(WDTO_2S);

	timer_init();
	measure_init();
	meascurr_init();
	meastemp_init();
	contrcurr_init();
	//TODO

	wdt_enable(WDTO_250MS);
	irq_enable();
	while (1) {
		wdt_reset();

		meascurr_work();
		meastemp_work();
		contrcurr_work();
		//TODO
	}
}
