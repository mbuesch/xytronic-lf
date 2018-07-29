#ifndef DEBUG_UART_H_
#define DEBUG_UART_H_

#include "util.h"
#include "fixpt.h"

#include <stdint.h>


#if CONF_DEBUG

#define DEBUG_PREFIX1(prefix0)		prefix0, NULL
#define DEBUG_PREFIX2(prefix0, prefix1)	prefix0, prefix1

#define DEBUG_PFX1(prefix0)		DEBUG_PREFIX1(PSTR(prefix0))
#define DEBUG_PFX2(prefix0, prefix1)	DEBUG_PREFIX2(PSTR(prefix0), PSTR(prefix1))


void debug_print_int32(const char __flash *prefix0,
		       const char __flash *prefix1,
		       int32_t value);
void debug_print_int24(const char __flash *prefix0,
		       const char __flash *prefix1,
		       int24_t value);
void debug_print_int16(const char __flash *prefix0,
		       const char __flash *prefix1,
		       int16_t value);
void debug_print_int8(const char __flash *prefix0,
		      const char __flash *prefix1,
		      int8_t value);
void debug_print_fixpt(const char __flash *prefix0,
		       const char __flash *prefix1,
		       fixpt_t value);

void debug_report_int32(const char __flash *prefix0,
			const char __flash *prefix1,
			int32_t *old_value,
			int32_t new_value);
void debug_report_int24(const char __flash *prefix0,
			const char __flash *prefix1,
			int24_t *old_value,
			int24_t new_value);
void debug_report_int16(const char __flash *prefix0,
			const char __flash *prefix1,
			int16_t *old_value,
			int16_t new_value);
void debug_report_int8(const char __flash *prefix0,
		       const char __flash *prefix1,
		       int8_t *old_value,
		       int8_t new_value);
void debug_report_fixpt(const char __flash *prefix0,
			const char __flash *prefix1,
			fixpt_t *old_value,
			fixpt_t new_value);

void debug_enable(bool enable);
bool debug_is_enabled(void);

void debug_uart_init(void);


#else /* CONF_DEBUG */


#define DEBUG_PREFIX1(prefix0)		NULL, NULL
#define DEBUG_PREFIX2(prefix0, prefix1)	NULL, NULL

#define DEBUG_PFX1(prefix0)		DEBUG_PREFIX1(NULL)
#define DEBUG_PFX2(prefix0, prefix1)	DEBUG_PREFIX2(NULL, NULL)


static inline
void debug_print_int32(const char __flash *prefix0,
		       const char __flash *prefix1,
		       int32_t value)
{
}

static inline
void debug_print_int24(const char __flash *prefix0,
		       const char __flash *prefix1,
		       int24_t value)
{
}

static inline
void debug_print_int16(const char __flash *prefix0,
		       const char __flash *prefix1,
		       int16_t value)
{
}

static inline
void debug_print_int8(const char __flash *prefix0,
		      const char __flash *prefix1,
		      int8_t value)
{
}

static inline
void debug_print_fixpt(const char __flash *prefix0,
		       const char __flash *prefix1,
		       fixpt_t value)
{
}

static inline
void debug_report_int32(const char __flash *prefix0,
			const char __flash *prefix1,
			int32_t *old_value,
			int32_t new_value)
{
}

static inline
void debug_report_int24(const char __flash *prefix0,
			const char __flash *prefix1,
			int24_t *old_value,
			int24_t new_value)
{
}

static inline
void debug_report_int16(const char __flash *prefix0,
			const char __flash *prefix1,
			int16_t *old_value,
			int16_t new_value)
{
}

static inline
void debug_report_int8(const char __flash *prefix0,
		       const char __flash *prefix1,
		       int8_t *old_value,
		       int8_t new_value)
{
}

static inline
void debug_report_fixpt(const char __flash *prefix0,
			const char __flash *prefix1,
			fixpt_t *old_value,
			fixpt_t new_value)
{
}

static inline
void debug_enable(bool enable)
{
}

static inline
bool debug_is_enabled(void)
{
	return false;
}

static inline
void debug_uart_init(void)
{
}


#endif /* CONF_DEBUG */
#endif /* DEBUG_UART_H_ */
