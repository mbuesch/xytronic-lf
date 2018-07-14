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
#include "display.h"

#include <avr/io.h>
#include <avr/wdt.h>


static bool debug_enabled;


static void debug_uart_enable(void)
{
	/* Enable UART transmitter. */
	UCSR0B |= (1 << TXEN0);
}

static void debug_uart_disable(void)
{
	/* Disable UART transmitter. */
	UCSR0B &= (uint8_t)~(1 << TXEN0);
}

static void debug_uart_tx(uint8_t byte)
{
	/* Wait for USART data register empty. */
	while (!(UCSR0A & (1 << UDRE0)))
		wdt_reset();

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

static void debug_uart_tx_eol(void)
{
	debug_uart_tx((uint8_t)'\r');
	debug_uart_tx((uint8_t)'\n');
}

static void debug_uart_print_timestamp(void)
{
	char buf[(sizeof(unsigned int) * 2) + 1];

	utoa((unsigned int)(uint16_t)timer_now(), buf, 16);
	debug_uart_tx_string(to_memx(&buf[0]));
	debug_uart_tx((uint8_t)':');
	debug_uart_tx((uint8_t)' ');
}

static void debug_print_str2(const char __flash *str0, const char *str1)
{
	uint8_t sreg;

	if (!debug_enabled)
		return;

	sreg = irq_disable_save();

	debug_uart_print_timestamp();
	if (str0)
		debug_uart_tx_string(to_memx(str0));
	if (str1) {
		debug_uart_tx((uint8_t)' ');
		debug_uart_tx_string(to_memx(str1));
	}
	debug_uart_tx_eol();

	irq_restore(sreg);
}

void debug_print_int32(const char __flash *prefix, int32_t value)
{
	char buf[10 + 1 + 1];

#if !defined(__CHECKER__) && !defined(SIMULATOR)
	build_assert(sizeof(long) == sizeof(value));
#endif
	ltoa(value, buf, 10);

	debug_print_str2(prefix, buf);
}

void debug_print_int24(const char __flash *prefix, int24_t value)
{
	debug_print_int32(prefix, value);
}

void debug_print_int16(const char __flash *prefix, int16_t value)
{
	debug_print_int32(prefix, value);
}

void debug_print_int8(const char __flash *prefix, int8_t value)
{
	debug_print_int32(prefix, value);
}

void debug_report_int32(const char __flash *prefix,
			int32_t *old_value,
			int32_t new_value)
{
	if (*old_value != new_value) {
		*old_value = new_value;
		debug_print_int32(prefix, new_value);
	}
}

void debug_report_int24(const char __flash *prefix,
			int24_t *old_value,
			int24_t new_value)
{
	if (*old_value != new_value) {
		*old_value = new_value;
		debug_print_int24(prefix, new_value);
	}
}

void debug_report_int16(const char __flash *prefix,
			int16_t *old_value,
			int16_t new_value)
{
	if (*old_value != new_value) {
		*old_value = new_value;
		debug_print_int16(prefix, new_value);
	}
}

void debug_report_int8(const char __flash *prefix,
		       int8_t *old_value,
		       int8_t new_value)
{
	if (*old_value != new_value) {
		*old_value = new_value;
		debug_print_int8(prefix, new_value);
	}
}

void debug_enable(bool enable)
{
	debug_enabled = enable;
	display_enable(!enable);
	if (enable) {
		debug_uart_enable();
		debug_uart_tx_string(to_memx(PSTR("\r\nst\r\n")));
	} else {
		debug_uart_disable();
	}
}

bool debug_is_enabled(void)
{
	return debug_enabled;
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
	debug_enable(false);
}
