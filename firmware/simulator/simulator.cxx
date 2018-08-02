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
#include "../menu.h"
#include "../measure_temp.h"

#include <thread>
#include <mutex>


//#define IRQ_ENDIS_DEBUG
//#define IRQ_HANDLER_DEBUG


#define memory_barrier()	__asm__ __volatile__("" : : : "memory")
#define ARRAY_SIZE(x)		(sizeof(x) / sizeof((x)[0]))
#define _stringify(x)		#x
#define stringify(x)		_stringify(x)


struct sim_context {
	/* Simulator context */
	int initialized;
	std::thread io_thread;
	volatile int io_thread_stop;
	bool mainloop_stats_ena;
	int mainloop_count;
	uint64_t mainloop_count_begin;
	double mainloops_per_sec;

	/* EEPROM */
	std::recursive_mutex eeprom_mutex;

	/* UART */
	uint8_t uart_tx_buf[4096];
	unsigned int uart_tx_buf_write;
	unsigned int uart_tx_buf_read;
	std::recursive_mutex uart_mutex;

	/* ADC */
	int ADC_vect_pending;
	int adc_conversion_running;
	uint64_t adc_conversion_start;
	uint16_t adc_values[11];
	std::recursive_mutex adc_mutex;

	/* Timer 0 */
	int TIMER0_COMPA_vect_pending;
	uint64_t timer0_prevsample;

	/* Interrupts */
	volatile int irq_suspend_request;
	volatile int irq_suspended;

	void reset()
	{
		initialized = 0;
		io_thread_stop = 0;
		mainloop_stats_ena = true;
		mainloop_count = 0;
		mainloop_count_begin = 0;
		mainloops_per_sec = -1.0;

		memset(uart_tx_buf, 0, sizeof(uart_tx_buf));
		uart_tx_buf_write = 0;
		uart_tx_buf_read = 0;

		ADC_vect_pending = 0;
		adc_conversion_running = 0;
		adc_conversion_start = 0;
		memset(adc_values, 0, sizeof(adc_values));

		TIMER0_COMPA_vect_pending = 0;
		timer0_prevsample = 0;

		irq_suspend_request = 0;
		irq_suspended = 0;
	}
};

struct sim_context sim;

/* Firmware EEPROM memory */
extern const uint8_t __start_eeprom;
extern const uint8_t __stop_eeprom;

/* Declare firmware interrupt service routines */
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
	return std::this_thread::get_id() == sim.io_thread.get_id();
}

void sim_irq_disable(void)
{
	memory_barrier();
	print_enter_irqendis("irq_disable");

	if (!this_is_io_thread()) {
		sim.irq_suspend_request = 1;
		while (!sim.irq_suspended && !sim.io_thread_stop)
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
		sim.irq_suspend_request = 0;
		while (sim.irq_suspended && !sim.io_thread_stop)
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

	std::lock_guard<std::recursive_mutex> locker(sim.eeprom_mutex);

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
	std::lock_guard<std::recursive_mutex> locker(sim.uart_mutex);

	sim.uart_tx_buf[sim.uart_tx_buf_write] = UDR0;
	if (++sim.uart_tx_buf_write >= ARRAY_SIZE(sim.uart_tx_buf))
		sim.uart_tx_buf_write = 0;
}

#define run_IRQ_vect(name) do {						\
		if (sim.name##_pending) {				\
			print_enter_irqhandler(stringify(name));	\
			sim.name##_pending = 0;				\
			name();						\
			print_exit_irqhandler(stringify(name));		\
		}							\
	} while (0)

/* Simulate timer0 hardware */
static void simulate_timer0(uint64_t systime_ms)
{
	if (TCCR0B & ((1 << CS02) | (1 << CS01) | (1 << CS00))) {
		uint64_t diff_ms = systime_ms - sim.timer0_prevsample;
		double count_per_ms = TIMERCPS / 1000.0;
		double call_count = diff_ms * count_per_ms;

		if (call_count >= 1.0) {
			sim.TIMER0_COMPA_vect_pending = 1;
			sim.timer0_prevsample = systime_ms;
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
		std::lock_guard<std::recursive_mutex> locker(sim.adc_mutex);

		switch (mux) {
		case MEAS_MUX_ADC0:
			adc_value = sim.adc_values[0];
			break;
		case MEAS_MUX_ADC1:
			adc_value = sim.adc_values[1];
			break;
		case MEAS_MUX_ADC2:
			adc_value = sim.adc_values[2];
			break;
		case MEAS_MUX_ADC3:
			adc_value = sim.adc_values[3];
			break;
		case MEAS_MUX_ADC4:
			adc_value = sim.adc_values[4];
			break;
		case MEAS_MUX_ADC5:
			adc_value = sim.adc_values[5];
			break;
		case MEAS_MUX_ADC6:
			adc_value = sim.adc_values[6];
			break;
		case MEAS_MUX_ADC7:
			adc_value = sim.adc_values[7];
			break;
		case MEAS_MUX_ADC8:
			adc_value = sim.adc_values[8];
			break;
		case MEAS_MUX_BG:
			adc_value = sim.adc_values[9];
			break;
		case MEAS_MUX_GND:
			adc_value = sim.adc_values[10];
			break;
		default:
			adc_value = 0;
			break;
		}
	}

	if (sim.adc_conversion_running) {
		if (ADCSRA & (1 << ADSC)) {
			if (systime_ms - sim.adc_conversion_start >= conv_runtime_ms) {
				sim.adc_conversion_running = 0;

				ADC = adc_value;
				if (ADCSRA & (1 << ADIE))
					sim.ADC_vect_pending = 1;
				if (!(ADCSRA & (1 << ADATE)))
					ADCSRA &= ~(1 << ADSC);
			}
		} else
			sim.adc_conversion_running = 0;
	} else {
		if (ADCSRA & (1 << ADSC)) {
			sim.adc_conversion_running = 1;
			sim.adc_conversion_start = systime_ms;
		}
	}
}

static void io_thread_func(void)
{
	uint64_t systime_ms;
	int suspended;

	systime_ms = systime_ms_get();
	sim.timer0_prevsample = systime_ms;

	while (!sim.io_thread_stop) {
		systime_ms = systime_ms_get();

		/* Run the simulated hardware */
		simulate_timer0(systime_ms);
		simulate_adc(systime_ms);

		/* Execute interrupt handlers.
		 * Note that in the simulator the IRQ handlers do actually
		 * run in parallel with the main thread outside of cli-sections. */
		suspended = sim.irq_suspend_request;
		sim.irq_suspended = suspended;
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
	std::lock_guard<std::recursive_mutex> locker(sim.uart_mutex);

	while (max_bytes > 0 && sim.uart_tx_buf_read != sim.uart_tx_buf_write) {
		*buf = sim.uart_tx_buf[sim.uart_tx_buf_read];
		count++;
		if (++sim.uart_tx_buf_read >= ARRAY_SIZE(sim.uart_tx_buf))
			sim.uart_tx_buf_read = 0;
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
	std::lock_guard<std::recursive_mutex> locker(sim.adc_mutex);

	if (adc_index >= 0 && adc_index < (int)ARRAY_SIZE(sim.adc_values)) {
		sim.adc_values[adc_index] = value;
		return true;
	}
	return false;
}

#define SETTINGS_ACCESS(_field, _compare_name, _name, _value, _write) ({ \
		bool retval = false;					\
		if (strcmp(_name, _compare_name) == 0) {		\
			if (_write) {					\
				get_settings()->_field = *(_value);	\
				store_settings();			\
			} else						\
				(*_value) = get_settings()->_field;	\
			retval = true;					\
		}							\
		retval;							\
	})

bool simulator_setting_access(const char *name, int *value, bool write)
{
	struct settings *s = get_settings();

	if (SETTINGS_ACCESS(temp_k[0].kp, "temp_k[0].kp", name, value, write) ||
	    SETTINGS_ACCESS(temp_k[0].ki, "temp_k[0].ki", name, value, write) ||
	    SETTINGS_ACCESS(temp_k[0].kd, "temp_k[0].kd", name, value, write) ||
	    SETTINGS_ACCESS(temp_k[0].d_decay_div, "temp_k[0].d_decay_div", name, value, write) ||
	    SETTINGS_ACCESS(temp_k[1].kp, "temp_k[1].kp", name, value, write) ||
	    SETTINGS_ACCESS(temp_k[1].ki, "temp_k[1].ki", name, value, write) ||
	    SETTINGS_ACCESS(temp_k[1].kd, "temp_k[1].kd", name, value, write) ||
	    SETTINGS_ACCESS(temp_k[1].d_decay_div, "temp_k[1].d_decay_div", name, value, write) ||
	    SETTINGS_ACCESS(temp_k[2].kp, "temp_k[2].kp", name, value, write) ||
	    SETTINGS_ACCESS(temp_k[2].ki, "temp_k[2].ki", name, value, write) ||
	    SETTINGS_ACCESS(temp_k[2].kd, "temp_k[2].kd", name, value, write) ||
	    SETTINGS_ACCESS(temp_k[2].d_decay_div, "temp_k[2].d_decay_div", name, value, write)) {
		contrtemp_update_pid_config();
		return true;
	}
	if (SETTINGS_ACCESS(temp_idle_setpoint, "temp_idle_setpoint", name, value, write)) {
		contrtemp_set_idle_setpoint(get_settings()->temp_idle_setpoint);
		return true;
	}
	if (SETTINGS_ACCESS(temp_setpoint[s->temp_setpoint_active], "temp_setpoint", name, value, write) ||
	    SETTINGS_ACCESS(temp_setpoint_active, "temp_setpoint_active", name, value, write)) {
		contrtemp_update_setpoint();
		menu_request_display_update();
		return true;
	}
	if (strcmp(name, "temp_adj") == 0) {
		if (write)
			meastemp_adjust_set(*value);
		else
			*value = meastemp_adjust_get();
		return true;
	}
	if (SETTINGS_ACCESS(serial, "serial", name, value, write))
		return true;
	return false;
}

void simulator_stats_ena(bool mainloop_stats_ena)
{
	sim.mainloop_stats_ena = mainloop_stats_ena;
	sim.mainloop_count = 0;
	sim.mainloop_count_begin = systime_ms_get();
	sim.mainloops_per_sec = -1.0;
}

void main_loop_once(void);

void simulator_mainloop_once(void)
{
	uint64_t begin = 0, end = 0, runtime = 0;

	if (!sim.initialized)
		return;

	if (sim.mainloop_stats_ena)
		begin = systime_ms_get();

	main_loop_once();

	if (sim.mainloop_stats_ena) {
		end = systime_ms_get();
		runtime = end - begin;

		if (++sim.mainloop_count >= 100) {
			double loops_per_sec;
			uint64_t duration;

			duration = end - sim.mainloop_count_begin;
			loops_per_sec = (double)(sim.mainloop_count * 1000) / (double)duration;

			sim.mainloops_per_sec = loops_per_sec;
			sim.mainloop_count = 0;
			sim.mainloop_count_begin = end;
		}

		printf("Mainloop core runtime %llu ms\n%.1lf loops per second wallclock\n",
		       (unsigned long long)runtime,
		       sim.mainloops_per_sec);
	}
}

void simulator_exit(void)
{
	if (!sim.initialized)
		return;

	sim.initialized = 0;
	printf("Exiting simulator...\n");
	sim.io_thread_stop = 1;
	sim.io_thread.join();
}

int program_main(void) _mainfunc;

bool simulator_init(void)
{
	if (sim.initialized)
		return false;

	sim.reset();
	simulator_stats_ena(true);
	fakeio_reset_all();

	/* Install I/O register hooks. */
	EEDR.set_read_hook(EEDR_read_hook);
	UCSR0A.set_read_hook(UCSR0A_read_hook);
	UDR0.set_write_hook(UDR0_write_hook);

	printf("Creating I/O-thread...\n");
	sim.io_thread = std::thread(io_thread_func);

	sim.initialized = 1;
	printf("Initializing firmware...\n");
	program_main();
	printf("Firmware initialized.\n");

	return true;
}
