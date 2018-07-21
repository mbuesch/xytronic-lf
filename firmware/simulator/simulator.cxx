/*
 * Xytronic LF-1600
 * Open Source firmware
 * Simulator core
 *
 * Copyright (c) 2018 Michael Buesch <m@bues.ch>
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

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <sys/time.h>

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "../timer.h"
#include "../measure.h"
#include "../settings.h"

#include <thread>
#include <mutex>


//#define IRQ_ENDIS_DEBUG
//#define IRQ_HANDLER_DEBUG


#define memory_barrier()	__asm__ __volatile__("" : : : "memory")
#define ARRAY_SIZE(x)		(sizeof(x) / sizeof((x)[0]))
#define _stringify(x)		#x
#define stringify(x)		_stringify(x)


/* Simulator context */
static int sim_initialized;
static std::thread io_thread;
static volatile int io_thread_stop;

/* EEPROM */
extern const uint8_t __start_eeprom;
extern const uint8_t __stop_eeprom;
static std::recursive_mutex eeprom_mutex;

/* UART */
static uint8_t uart_tx_buf[4096];
static unsigned int uart_tx_buf_write;
static unsigned int uart_tx_buf_read;
static std::recursive_mutex uart_mutex;

/* ADC */
static int ADC_vect_pending;
static int adc_conversion_running;
static uint64_t adc_conversion_start;
static uint16_t adc_values[11];
static std::recursive_mutex adc_mutex;

/* Timer 0 */
static int TIMER0_COMPA_vect_pending;
static uint64_t timer0_prevsample;

/* Interrupts */
static volatile int irq_suspend_request;
static volatile int irq_suspended;
ISR(ADC_vect);
ISR(EE_READY_vect);
ISR(TIMER0_COMPA_vect);


#define print_enter(name)	printf(">>> enter %s()\n", name)
#define print_exit(name)	printf("<<< exit %s()\n", name)

#ifdef IRQ_HANDLER_DEBUG
# define print_enter_irqhandler(name)	print_enter(name)
# define print_exit_irqhandler(name)	print_exit(name)
#else
# define print_enter_irqhandler(name)	do { } while (0)
# define print_exit_irqhandler(name)	do { } while (0)
#endif

#ifdef IRQ_ENDIS_DEBUG
# define print_enter_irqendis(name)	print_enter(name)
# define print_exit_irqendis(name)	print_exit(name)
#else
# define print_enter_irqendis(name)	do { } while (0)
# define print_exit_irqendis(name)	do { } while (0)
#endif


static bool this_is_io_thread()
{
	return std::this_thread::get_id() == io_thread.get_id();
}

void sim_irq_disable(void)
{
	memory_barrier();
	print_enter_irqendis("irq_disable");

	if (!this_is_io_thread()) {
		irq_suspend_request = 1;
		while (!irq_suspended && !io_thread_stop)
			std::this_thread::yield();

		SREG &= (uint8_t)~(1u << SREG_I);
	}

	print_exit_irqendis("irq_disable");
	memory_barrier();
}

void sim_irq_enable(void)
{
	memory_barrier();
	print_enter_irqendis("irq_enable");

	if (!this_is_io_thread()) {
		irq_suspend_request = 0;
		while (irq_suspended && !io_thread_stop)
			std::this_thread::yield();

		SREG |= (1u << SREG_I);
	}

	print_exit_irqendis("irq_enable");
	memory_barrier();
}

uint8_t sim_irq_disable_save(void)
{
	uint8_t sreg_flags;

	memory_barrier();
	print_enter_irqendis("irq_disable_save");

	sreg_flags = SREG;
	sim_irq_disable();

	print_exit_irqendis("irq_disable_save");
	memory_barrier();

	return sreg_flags;
}

void sim_irq_restore(uint8_t sreg_flags)
{
	memory_barrier();
	print_enter_irqendis("sim_irq_restore");

	if (sreg_flags & (1u << SREG_I))
		sim_irq_enable();

	print_exit_irqendis("sim_irq_restore");
	memory_barrier();
}

static uint64_t systime_ms_get(void)
{
	struct timeval tv = {};
	int err;
	uint64_t ms;

	err = gettimeofday(&tv, NULL);
	if (err)
		perror("gettimeofday()");

	ms = (uint64_t)tv.tv_sec * 1000u;
	ms += (uint64_t)tv.tv_usec / 1000u;

	return ms;
}

static void EEDR_read_hook(FakeIO<uint8_t> &io)
{
	uint8_t data;
	ee_addr_t addr;
	uint16_t offset;
	uint8_t *ee_pointer;

	std::lock_guard<std::recursive_mutex> locker(eeprom_mutex);

	if (EECR & (1 << EERE)) {
		addr = EEAR;
		ee_pointer = (uint8_t *)addr;
		offset = ee_pointer - &__start_eeprom;

		data = (&__start_eeprom)[offset];
#if 0
		printf("sim_ee_read_hook() at 0x%X = 0x%02X\n",
		       (unsigned int)offset, (unsigned int)data);
#endif
		EEDR = data;
		EECR &= ~(1 << EERE);
	}
}

static void UCSR0A_read_hook(FakeIO<uint8_t> &io)
{
	UCSR0A |= (1 << UDRE0);
}

static void UDR0_write_hook(FakeIO<uint8_t> &io, uint8_t prev_value)
{
	std::lock_guard<std::recursive_mutex> locker(uart_mutex);

	uart_tx_buf[uart_tx_buf_write] = UDR0;
	if (++uart_tx_buf_write >= ARRAY_SIZE(uart_tx_buf))
		uart_tx_buf_write = 0;
}

#define run_IRQ_vect(name) do {						\
		if (name##_pending) {					\
			print_enter_irqhandler(stringify(name));	\
			name##_pending = 0;				\
			name();						\
			print_exit_irqhandler(stringify(name));		\
		}							\
	} while (0)

/* Simulate timer0 hardware */
static void simulate_timer0(uint64_t systime_ms)
{
	if (TCCR0B & ((1 << CS02) | (1 << CS01) | (1 << CS00))) {
		uint64_t diff_ms = systime_ms - timer0_prevsample;
		double count_per_ms = TIMERCPS / 1000.0;
		double call_count = diff_ms * count_per_ms;

		if (call_count >= 1.0) {
			TIMER0_COMPA_vect_pending = 1;
			timer0_prevsample = systime_ms;
		}
	}
}

/* Simulate ADC hardware */
static void simulate_adc(uint64_t systime_ms)
{
	uint16_t adc_value;
	uint32_t adc_hz, adc_ps;
	uint64_t conv_runtime_ms;
	uint8_t mux;

	switch (ADCSRA & ((1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0))) {
	case MEAS_PS_2:
		adc_ps = 2;
		break;
	case MEAS_PS_4:
		adc_ps = 4;
		break;
	case MEAS_PS_8:
		adc_ps = 8;
		break;
	case MEAS_PS_16:
		adc_ps = 16;
		break;
	case MEAS_PS_32:
		adc_ps = 32;
		break;
	case MEAS_PS_64:
		adc_ps = 64;
		break;
	case MEAS_PS_128:
		adc_ps = 128;
		break;
	default:
		adc_ps = 1;
		break;
	}
	adc_hz = F_CPU / adc_ps;
	conv_runtime_ms = (uint64_t)ceil((1000.0 / adc_hz) * 13);

	mux = (ADMUX & ((1 << MUX3) | (1 << MUX2) | (1 << MUX1) | (1 << MUX0)));
	{
		std::lock_guard<std::recursive_mutex> locker(adc_mutex);

		switch (mux) {
		case MEAS_MUX_ADC0:
			adc_value = adc_values[0];
			break;
		case MEAS_MUX_ADC1:
			adc_value = adc_values[1];
			break;
		case MEAS_MUX_ADC2:
			adc_value = adc_values[2];
			break;
		case MEAS_MUX_ADC3:
			adc_value = adc_values[3];
			break;
		case MEAS_MUX_ADC4:
			adc_value = adc_values[4];
			break;
		case MEAS_MUX_ADC5:
			adc_value = adc_values[5];
			break;
		case MEAS_MUX_ADC6:
			adc_value = adc_values[6];
			break;
		case MEAS_MUX_ADC7:
			adc_value = adc_values[7];
			break;
		case MEAS_MUX_ADC8:
			adc_value = adc_values[8];
			break;
		case MEAS_MUX_BG:
			adc_value = adc_values[9];
			break;
		case MEAS_MUX_GND:
			adc_value = adc_values[10];
			break;
		default:
			adc_value = 0;
			break;
		}
	}

	if (adc_conversion_running) {
		if (ADCSRA & (1 << ADSC)) {
			if (systime_ms - adc_conversion_start >= conv_runtime_ms) {
				adc_conversion_running = 0;

				ADC = adc_value;
				if (ADCSRA & (1 << ADIE))
					ADC_vect_pending = 1;
				if (!(ADCSRA & (1 << ADATE)))
					ADCSRA &= ~(1 << ADSC);
			}
		} else
			adc_conversion_running = 0;
	} else {
		if (ADCSRA & (1 << ADSC)) {
			adc_conversion_running = 1;
			adc_conversion_start = systime_ms;
		}
	}
}

static void io_thread_func(void)
{
	uint64_t systime_ms;
	int suspended;

	systime_ms = systime_ms_get();
	timer0_prevsample = systime_ms;

	while (!io_thread_stop) {
		systime_ms = systime_ms_get();

		/* Run the simulated hardware */
		simulate_timer0(systime_ms);
		simulate_adc(systime_ms);

		/* Execute interrupt handlers.
		 * Note that in the simulator the IRQ handlers do actually
		 * run in parallel with the main thread outside of cli-sections. */
		suspended = irq_suspend_request;
		irq_suspended = suspended;
		if (!suspended) {
			run_IRQ_vect(ADC_vect);
			run_IRQ_vect(TIMER0_COMPA_vect);
		}

		_delay_us(500);
	}
}

size_t simulator_uart_get_tx(uint8_t *buf, size_t max_bytes)
{
	size_t count = 0;
	std::lock_guard<std::recursive_mutex> locker(uart_mutex);

	while (max_bytes > 0 && uart_tx_buf_read != uart_tx_buf_write) {
		*buf = uart_tx_buf[uart_tx_buf_read];
		count++;
		if (++uart_tx_buf_read >= ARRAY_SIZE(uart_tx_buf))
			uart_tx_buf_read = 0;
		max_bytes--;
		buf++;
	}

	return count;
}

bool simulator_pwm_get(int pwm_index, uint16_t *value, uint16_t *max_value)
{
	switch (pwm_index) {
	case 1:
		*value = OCR1A;
		*max_value = ICR1;
		return true;
	}
	return false;
}

bool simulator_adc_set(int adc_index, uint16_t value)
{
	std::lock_guard<std::recursive_mutex> locker(adc_mutex);

	if (adc_index >= 0 && adc_index < (int)ARRAY_SIZE(adc_values)) {
		adc_values[adc_index] = value;
		return true;
	}
	return false;
}

#define SETTINGS_ACCESS(_field, _name, _value, _write) do {		\
		if (strcmp(_name, stringify(_field)) == 0) {		\
			if (_write)					\
				get_settings()->_field = *(_value);	\
			else						\
				(*_value) = get_settings()->_field;	\
			return true;					\
		}							\
	} while (0)

bool simulator_setting_access(const char *name, int *value, bool write)
{
	SETTINGS_ACCESS(temp_k[0].kp, name, value, write);
	SETTINGS_ACCESS(temp_k[0].ki, name, value, write);
	SETTINGS_ACCESS(temp_k[0].kd, name, value, write);
	SETTINGS_ACCESS(temp_k[0].d_decay_div, name, value, write);
	SETTINGS_ACCESS(temp_k[1].kp, name, value, write);
	SETTINGS_ACCESS(temp_k[1].ki, name, value, write);
	SETTINGS_ACCESS(temp_k[1].kd, name, value, write);
	SETTINGS_ACCESS(temp_k[1].d_decay_div, name, value, write);
	SETTINGS_ACCESS(temp_k[2].kp, name, value, write);
	SETTINGS_ACCESS(temp_k[2].ki, name, value, write);
	SETTINGS_ACCESS(temp_k[2].kd, name, value, write);
	SETTINGS_ACCESS(temp_k[2].d_decay_div, name, value, write);
	SETTINGS_ACCESS(temp_idle_setpoint, name, value, write);
	SETTINGS_ACCESS(temp_setpoint[0], name, value, write);
	SETTINGS_ACCESS(temp_setpoint[1], name, value, write);
	SETTINGS_ACCESS(temp_setpoint[2], name, value, write);
	SETTINGS_ACCESS(temp_setpoint[3], name, value, write);
	SETTINGS_ACCESS(temp_setpoint[4], name, value, write);
	SETTINGS_ACCESS(temp_setpoint[5], name, value, write);
	SETTINGS_ACCESS(temp_setpoint_active, name, value, write);
	SETTINGS_ACCESS(temp_adj, name, value, write);
	SETTINGS_ACCESS(serial, name, value, write);
	return false;
}

void main_loop_once(void);

void simulator_mainloop_once(void)
{
	if (!sim_initialized)
		return;

	main_loop_once();
}

void simulator_exit(void)
{
	if (!sim_initialized)
		return;

	sim_initialized = 0;
	printf("Exiting simulator...\n");
	io_thread_stop = 1;
	io_thread.join();
}

int program_main(void) _mainfunc;

bool simulator_init(void)
{
	if (sim_initialized)
		return false;

	/* Install I/O register hooks. */
	EEDR.set_read_hook(EEDR_read_hook);
	UCSR0A.set_read_hook(UCSR0A_read_hook);
	UDR0.set_write_hook(UDR0_write_hook);

	printf("Creating I/O-thread...\n");
	io_thread = std::thread(io_thread_func);

	sim_initialized = 1;
	printf("Executing program...\n");
	program_main();

	return true;
}
