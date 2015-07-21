#ifndef MY_UTIL_H_
#define MY_UTIL_H_

#ifndef F_CPU
# warning "F_CPU not defined"
#endif
#include <util/delay.h>

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <avr/cpufunc.h>


/* Return the smaller value of 'a' and 'b'. */
#define min(a, b)	({				\
		__typeof__(a) __amin = (a);		\
		__typeof__(b) __bmin = (b);		\
		__amin < __bmin ? __amin : __bmin;	\
	})

/* Return the bigger value of 'a' and 'b'. */
#define max(a, b)	({				\
		__typeof__(a) __amax = (a);		\
		__typeof__(b) __bmax = (b);		\
		__amax > __bmax ? __amax : __bmax;	\
	})

/* Return 'value' clamped inbetween 'min_val' and 'max_val'. */
#define clamp(value, min_val, max_val)		\
	max(min(value, max_val), min_val)

/* Return the absolute value of 'val' */
#undef abs
#define abs(val)	({			\
		__typeof__(val) __val = (val);	\
		__val >= 0 ? __val : -__val;	\
	})

/* Round an integer 'n' up to a multiple of 's'. */
#define round_up(n, s)          ((((n) + (s) - 1) / (s)) * (s))

/* Perform a ceiling division of 'x' / 'd'. */
#define div_round_up(x, d)		({	\
		__typeof__(x) __x = (x);	\
		__typeof__(d) __d = (d);	\
		(__x + __d - 1) / __d;		\
	})

/* Divide unsigned integer x by unsigned integer d and
 * round to the closest result. */
#define div_round(x, d)	({			\
		__typeof__(x) __x = (x);	\
		__typeof__(d) __d = (d);	\
		(__x + (__d / 2)) / __d;	\
	})

/* Swap values 'a' and 'b' in place.
 * Also checks whether 'a' and 'b' are of the same type. */
#define swap_values(a, b)	do {		\
		__typeof__(a) __a = (a);	\
		__typeof__(b) __b = (b);	\
		__typeof__(a) __tmp = __a;	\
		(void)((&__a) == (&__b));	\
		(a) = __b;			\
		(b) = __tmp;			\
	} while (0)

/* Helper to efficiently convert a bit number to
 * the corresponding bit mask.
 * Resolves at compile-time, if bitnr is a compile-time constant.
 * Uses an efficient table lookup otherwise.
 */
extern const uint8_t PROGMEM _bit_to_mask8[8];
#define BITMASK8(bitnr)		(			\
		__builtin_constant_p(bitnr) ?		\
		(1 << (bitnr)) :			\
		pgm_read_byte(&_bit_to_mask8[(bitnr)])	\
	)

/* Return the number of elements in a C array. */
#define ARRAY_SIZE(x)		(sizeof(x) / sizeof((x)[0]))

/* Progmem pointer annotation. */
#define PROGPTR			/* progmem pointer */

/* Memory barrier. */
#define mb()			__asm__ __volatile__("" : : : "memory")

/* Do-not-inline function attribute. */
#define noinline		__attribute__((__noinline__))

/* Pure-function attribute. */
#define pure_fn			__attribute__((__pure__))

/* Const-function attribute. */
#define const_fn		__attribute__((__const__))

/* Data structure packing attribute. */
#define _packed			__attribute__((__packed__))

/* Build-time assertion.
 * 'cond' must be a compile-time constant.
 * Build will fail, if 'cond' is false.
 */
#define build_assert(cond)	((void)sizeof(char[1 - 2 * !(cond)]))

/* Code flow attributes */
#define noreturn		__attribute__((__noreturn__))
#define _mainfunc		__attribute__((__OS_main__))
#if defined(__GNUC__) && __GNUC__ >= 4 && __GNUC_MINOR__ >= 5
# define unreachable()		__builtin_unreachable()
#else
# define unreachable()		while (1)
#endif


void reboot(void) noreturn;
void panic(void) noreturn;
#define PANIC_ON(condition)	do { if (condition) panic(); } while (0)


void eeprom_read_block_wdtsafe(void *_dst, const void *_src, size_t n);
void eeprom_update_block_wdtsafe(const void *_src, void *dst, size_t n);


/* Disable interrupts globally. */
static inline void irq_disable(void)
{
	cli();
	mb();
}

/* Enable interrupts globally. */
static inline void irq_enable(void)
{
	mb();
	sei();
}

/* Save flags and disable interrupts globally. */
static inline uint8_t irq_disable_save(void)
{
	uint8_t sreg = SREG;
	cli();
	mb();
	return sreg;
}

/* Restore interrupt flags. */
static inline void irq_restore(uint8_t sreg_flags)
{
	mb();
	SREG = sreg_flags;
}

/* Check whether the interrupt-enable flag is set in 'sreg_flags' */
static inline bool __irqs_enabled(uint8_t sreg_flags)
{
	return !!(sreg_flags & (1 << SREG_I));
}

/* Check whether interrupts are enabled globally. */
static inline bool irqs_enabled(void)
{
	return __irqs_enabled(SREG);
}

#endif /* MY_UTIL_H_ */
