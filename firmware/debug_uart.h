#ifndef DEBUG_UART_H_
#define DEBUG_UART_H_

#include "util.h"

#include <stdint.h>


void debug_print_int32(const char __flash *prefix, int32_t value);
void debug_print_int24(const char __flash *prefix, int24_t value);
void debug_print_int16(const char __flash *prefix, int16_t value);

void debug_report_int32(const char __flash *prefix,
			int32_t *old_value,
			int32_t new_value);
void debug_report_int24(const char __flash *prefix,
			int24_t *old_value,
			int24_t new_value);
void debug_report_int16(const char __flash *prefix,
			int16_t *old_value,
			int16_t new_value);

void debug_enable(bool enable);
bool debug_is_enabled(void);

void debug_uart_init(void);

#endif /* DEBUG_UART_H_ */
