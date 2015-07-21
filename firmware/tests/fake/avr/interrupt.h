#include "avr/io.h"

#define cli()	__asm__ __volatile__("" : : : "memory")
#define sei()	__asm__ __volatile__("" : : : "memory")
