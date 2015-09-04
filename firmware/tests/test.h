#include "fake/fake.h"

#include <stdlib.h>
#include <stdio.h>


#define DO_STRINGIFY(x)	#x
#define STRINGIFY(x)	DO_STRINGIFY(x)


#define CHECK(cond)	do {						\
		if (!(cond)) {						\
			fprintf(stderr,					\
				"\nTest condition\n"			\
				"    %s\n"				\
				"at %s:%d\n"				\
				"FAILED\n\n",				\
				STRINGIFY(cond),			\
				__FILE__, __LINE__);			\
			exit(1);					\
		}							\
	} while (0)

#define CHECK_EQ_II(a, b)	do {					\
		if (!((a) == (b))) {					\
			fprintf(stderr,					\
				"\nTest condition\n"			\
				"    %s == %s\n"			\
				"evaluated as\n"			\
				"    %d == %d\n"			\
				"at %s:%d\n"				\
				"FAILED\n\n",				\
				STRINGIFY(a), STRINGIFY(b),		\
				(int)(a), (int)(b),			\
				__FILE__, __LINE__);			\
			exit(1);					\
		}							\
	} while (0)

#define CHECK_EQ_UU(a, b)	do {					\
		if (!((a) == (b))) {					\
			fprintf(stderr,					\
				"\nTest condition\n"			\
				"    %s == %s\n"			\
				"evaluated as\n"			\
				"    %u == %u\n"			\
				"at %s:%d\n"				\
				"FAILED\n\n",				\
				STRINGIFY(a), STRINGIFY(b),		\
				(unsigned int)(a), (unsigned int)(b),	\
				__FILE__, __LINE__);			\
			exit(1);					\
		}							\
	} while (0)
