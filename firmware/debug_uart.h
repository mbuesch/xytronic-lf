#ifndef DEBUG_UART_H_
#define DEBUG_UART_H_

#include <stdint.h>


struct report_int16_context {
	int16_t old_value;
};

void debug_print(const char __memx *str);
void debug_print_int16(const char __memx *prefix, int16_t value);
void debug_report_int16(struct report_int16_context *ctx,
			const char __memx *prefix,
			int16_t new_value);
void debug_uart_init(void);

#endif /* DEBUG_UART_H_ */
