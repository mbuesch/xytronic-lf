#ifndef FILTER_H_
#define FILTER_H_

#include "util.h"
#include "fixpt.h"


struct lp_filter_u16 {
	uint24_t filter_buf;
};

struct lp_filter_fixpt {
	fixpt_t filter_buf;
};


static inline void lp_filter_u16_set(struct lp_filter_u16 *lp,
				     uint16_t new_value)
{
	lp->filter_buf = new_value;
}

static inline void lp_filter_u16_reset(struct lp_filter_u16 *lp)
{
	lp_filter_u16_set(lp, 0u);
}

uint16_t lp_filter_u16_run(struct lp_filter_u16 *lp, uint16_t in,
			   uint8_t filter_shift);

static inline void lp_filter_fixpt_set(struct lp_filter_fixpt *lp,
				       fixpt_t new_value)
{
	lp->filter_buf = new_value;
}

static inline void lp_filter_fixpt_reset(struct lp_filter_fixpt *lp)
{
	lp_filter_fixpt_set(lp, int_to_fixpt(0));
}

fixpt_t lp_filter_fixpt_run(struct lp_filter_fixpt *lp, fixpt_t in,
			    fixpt_t div);

#endif /* FILTER_H_ */
