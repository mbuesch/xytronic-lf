#include <stdlib.h>
#include <stdio.h>


#define DO_STRINGIFY(x)	#x
#define STRINGIFY(x)	DO_STRINGIFY(x)


#define CHECK(cond)	do {						\
		if (!(cond)) {						\
			fprintf(stderr,					\
				"Test condition at %s:%d failed:\n"	\
				"    %s\n",				\
				__FILE__, __LINE__, STRINGIFY(cond));	\
			exit(1);					\
		}							\
	} while (0)
