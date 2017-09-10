/*
 * Xytronic LF-1600
 * Open Source firmware
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

#include "main.h"
#include "measure.h"
#include "measure_current.h"
#include "measure_temp.h"
#include "controller_current.h"
#include "controller_temp.h"
#include "pwm_current.h"
#include "timer.h"
#include "util.h"
#include "debug_uart.h"
#include "display.h"
#include "buttons.h"
#include "menu.h"
#include "settings.h"
#include "presets.h"

#include <avr/io.h>
#include <avr/wdt.h>


/* Configure unused package pins. */
static void unused_pins_init(void)
{
	/* Configure unused pins as inputs with pull up
	 * to get a well defined level on the input driver.
	 */

	/* PB5 */
	DDRB &= (uint8_t)~(1u << DDB5);
	PORTB |= 1u << PB5;

	/* PB6 */
	DDRB &= (uint8_t)~(1u << DDB6);
	PORTB |= 1u << PB6;

	/* PC0 */
	DDRC &= (uint8_t)~(1u << DDC0);
	PORTC |= 1u << PC0;
}

/* Early boot routine. Runs before main().
 * This function does not have a stack frame.
 */
void early_init(void) __attribute__((naked, section(".init3"), used));
void early_init(void)
{
	/* Reconfigure watchdog. */
	MCUSR = 0;
	wdt_enable(WDTO_2S);
}

/* Main entry point. */
int main(void) _mainfunc;
int main(void)
{
	/* Wait a bit for the capacitors to charge
	 * and the reference voltage to stabilize.
	 */
	_delay_ms(300);

	/* Initialize basic system functions. */
	timer_init();
	buttons_init();
	debug_uart_init();
	settings_init();
	display_init();

	/* Initialize the measurement subsystem */
	measure_init();
	meascurr_init();
	meastemp_init();

	/* Initialize the controllers. */
	contrcurr_init();
	presets_init();
	contrtemp_init();

	/* Initialize the current actuator. */
	pwmcurr_init();

	/* Initialize the user interface. */
	menu_init();

	/* Configure unused package pins. */
	unused_pins_init();

	/* Startup measurements to get everything going. */
	measure_start();

	wdt_enable(WDTO_250MS);
	irq_enable();
	while (1) {
		wdt_reset();

		/* Handle measurement results (if any).
		 * This will also run the controllers, if needed.
		 */
		measure_work();

		/* Handle user interface events. */
		menu_work();
		display_work();
		buttons_work();
		settings_work();
	}
}
