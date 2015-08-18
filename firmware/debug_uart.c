/*
 * Debugging UART interface
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

#include "debug_uart.h"
#include "util.h"
#include "timer.h"
#include "buttons.h"

#include <avr/io.h>


enum {
	OLDSTATE_TXDDR	= 1 << 0,
	OLDSTATE_TXPORT	= 1 << 1,
};

#define TX_DDR		DDRD
#define TX_DDRBIT	DDD1
#define TX_PORT		PORTD
#define TX_PORTBIT	PD1


static bool debug_enabled;


static uint8_t debug_uart_enable(void)
{
	uint8_t old_state = 0;

	/* Save TX pin DDR and PORT settings. */
	if (TX_DDR & (1 << TX_DDRBIT))
		old_state |= OLDSTATE_TXDDR;
	if (TX_PORT & (1 << TX_PORTBIT))
		old_state |= OLDSTATE_TXPORT;

	/* Set TX pin as output */
	TX_PORT &= (uint8_t)~(1 << TX_PORTBIT);
	TX_DDR |= (1 << TX_DDRBIT);

	/* Enable UART transmitter. */
	UCSR0B |= (1 << TXEN0);

	return old_state;
}

static void debug_uart_disable(uint8_t old_state)
{
	/* Disable UART transmitter. */
	UCSR0B &= (uint8_t)~(1 << TXEN0);

	/* Restore TX pin DDR and PORT settings. */
	if (old_state & OLDSTATE_TXDDR)
		TX_DDR |= (1 << TX_DDRBIT);
	else
		TX_DDR &= (uint8_t)~(1 << TX_DDRBIT);
	if (old_state & OLDSTATE_TXPORT)
		TX_PORT |= (1 << TX_PORTBIT);
	else
		TX_PORT &= (uint8_t)~(1 << TX_PORTBIT);
}

static void debug_uart_tx(uint8_t byte)
{
	/* Wait for USART data register empty. */
	while (!(UCSR0A & (1 << UDRE0)));

	/* Transmit the byte. */
	UDR0 = byte;
}

static void debug_uart_tx_string(const char __memx *str)
{
	char c;

	while (1) {
		c = *str;
		if (c == '\0')
			break;
		debug_uart_tx((uint8_t)c);
		str++;
	}
}

static void debug_uart_print_timestamp(void)
{
	char buf[(sizeof(unsigned int) / 4) + 1];

	utoa((unsigned int)(uint16_t)_timer_get_now(), buf, 16);
	debug_uart_tx_string(buf);
	debug_uart_tx(':');
	debug_uart_tx(' ');
}

void debug_print(const char __memx *str)
{
	uint8_t sreg;
	uint8_t old_state;

	if (!debug_enabled)
		return;

	sreg = irq_disable_save();
	old_state = debug_uart_enable();

	debug_uart_print_timestamp();
	debug_uart_tx_string(str);
	debug_uart_tx((uint8_t)'\n');

	debug_uart_disable(old_state);
	irq_restore(sreg);
}

void debug_print_int16(const char __memx *prefix, int16_t value)
{
	uint8_t sreg;
	uint8_t old_state;
	char buf[5 + 1 + 1];

	if (!debug_enabled)
		return;

	sreg = irq_disable_save();
	old_state = debug_uart_enable();

	debug_uart_print_timestamp();
	debug_uart_tx_string(prefix);
	debug_uart_tx((uint8_t)' ');
	build_assert(sizeof(int) == sizeof(value));
	itoa(value, buf, 10);
	debug_uart_tx_string(buf);
	debug_uart_tx((uint8_t)'\n');

	debug_uart_disable(old_state);
	irq_restore(sreg);
}

void debug_report_int16(struct report_int16_context *ctx,
			const char __memx *prefix,
			int16_t new_value)
{
	if (ctx->old_value != new_value) {
		ctx->old_value = new_value;
		debug_print_int16(prefix, new_value);
	}
}

void debug_uart_init(void)
{
	build_assert(F_CPU == 8000000ul);

	/* 9600 baud */
	UBRR0 = 51;
	UCSR0A = (1 << TXC0) | (0 << U2X0) | (0 << MPCM0);
	UCSR0B = (0 << RXCIE0) | (0 << TXCIE0) | (0 << UDRIE0) |
		 (0 << RXEN0) | (0 << TXEN0) |
		 (0 << UCSZ02);
	UCSR0C = (0 << UMSEL01) | (0 << UMSEL00) |
		 (0 << UPM01) | (0 << UPM00) |
		 (1 << USBS0) |
		 (1 << UCSZ01) | (1 << UCSZ00);

	/* Enable debugging, if requested via input pins. */
	if (buttons_debug_requested())
		debug_enabled = 1;
	else
		debug_enabled = 0;
}
