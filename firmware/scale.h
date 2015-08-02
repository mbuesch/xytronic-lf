#ifndef SCALE_UNSCALE_H_
#define SCALE_UNSCALE_H_

#include <stdint.h>

#include "fixpt.h"


fixpt_t scale(int16_t raw, int16_t raw_lo, int16_t raw_hi,
	      fixpt_t phys_lo, fixpt_t phys_hi);

int16_t unscale(fixpt_t phys, fixpt_t phys_lo, fixpt_t phys_hi,
		int16_t raw_lo, int16_t raw_hi);

#endif /* SCALE_UNSCALE_H_ */
