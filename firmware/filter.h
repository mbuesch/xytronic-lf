#ifndef FILTER_H_
#define FILTER_H_

#include "util.h"


struct lp_filter_u16 {
	uint24_t filter_buf;
};


static inline void lp_filter_u16_reset(struct lp_filter_u16 *lp)
{
	lp->filter_buf = 0u;
}

uint16_t lp_filter_u16_run(struct lp_filter_u16 *lp, uint16_t in,
			   uint8_t filter_shift);

#endif /* FILTER_H_ */
