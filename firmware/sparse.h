#ifndef SPARSE_H_
#define SPARSE_H_

#ifdef __CHECKER__

/* Dummy declarations for sparse checker. */

#define __flash1			__attribute__((address_space(1)))
#define __flash2			__attribute__((address_space(2)))
#define __flash3			__attribute__((address_space(3)))
#define __flash4			__attribute__((address_space(4)))
#define __flash5			__attribute__((address_space(5)))
#define __flash				__attribute__((address_space(6)))
#define __memx				__attribute__((address_space(7)))
#define __ATTR_PROGMEM__		__flash
#define __OS_main__			dllexport
#define __builtin_avr_delay_cycles(x)

#define _SYS_CDEFS_H			/* Suppress system cdefs.h */

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <avr/pgmspace.h>

#define __force				__attribute__((force))
#undef PSTR
#define PSTR(x)				((typeof(*x) __flash * __force)(x))
#define to_memx(x)			((typeof(*x) __memx * __force)(x))
#undef FDEV_SETUP_STREAM
#define FDEV_SETUP_STREAM(p, g, f)	{}

typedef int32_t		__int24;
typedef uint32_t	__uint24;

extern void _delay_us(double x);
extern void _delay_ms(double x);
extern void _delay_loop_1(uint8_t x);
extern void _delay_loop_2(uint16_t x);

#else /* __CHECKER__ */

#define to_memx(x)	(x)

#endif /* __CHECKER__ */
#endif /* SPARSE_H_ */
