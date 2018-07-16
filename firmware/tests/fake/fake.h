#ifndef FAKE_H_
#define FAKE_H_

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>


/* This __(u)int24 is 32 bits wide. This might cause problems. */
typedef uint32_t __uint24;
typedef int32_t __int24;

static inline char *utoa(unsigned int val, char *s, int radix)
{
	if (radix == 16) {
		sprintf(s, "%X", val);
	} else {
		fprintf(stderr, "utoa: Unsupported radix %d\n", radix);
		abort();
	}
	return s;
}

static inline char *ltoa(long val, char *s, int radix)
{
	if (radix == 10) {
		sprintf(s, "%ld", val);
	} else {
		fprintf(stderr, "ltoa: Unsupported radix %d\n", radix);
		abort();
	}
	return s;
}

#endif /* FAKE_H_ */
