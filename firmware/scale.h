#ifndef SCALE_H_
#define SCALE_H_

#include <stdint.h>

#include "fixpt.h"


fixpt_t scale(int16_t raw, int16_t raw_lo, int16_t raw_hi,
	      fixpt_t phys_lo, fixpt_t phys_hi);

int16_t unscale(fixpt_t phys, fixpt_t phys_lo, fixpt_t phys_hi,
		int16_t raw_lo, int16_t raw_hi);

fixpt_t rescale(fixpt_t phys0, fixpt_t phys0_lo, fixpt_t phys0_hi,
		fixpt_t phys1_lo, fixpt_t phys1_hi);

#endif /* SCALE_H_ */
