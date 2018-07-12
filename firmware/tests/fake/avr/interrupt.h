#include "avr/io.h"

#ifndef _cli_handler
# define _cli_handler()  do { } while (0)
#endif

#ifndef _sei_handler
# define _sei_handler()  do { } while (0)
#endif

#define cli()	do {						\
		__asm__ __volatile__("" : : : "memory");	\
		_cli_handler();					\
		__asm__ __volatile__("" : : : "memory");	\
	} while (0)

#define sei()	do {						\
		__asm__ __volatile__("" : : : "memory");	\
		_sei_handler();					\
		__asm__ __volatile__("" : : : "memory");	\
	} while (0)

#define ISR(name)	void name(void)
