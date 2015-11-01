#ifndef CALIB_CURRENT_H_
#define CALIB_CURRENT_H_

#include "util.h"

#if CONF_CALIB

void calcurr_work(void);

bool calcurr_is_enabled(void);
void calcurr_set_enabled(bool enable);

#else /* CONF_CALIB */

static inline void calcurr_work(void)
{
}

static inline bool calcurr_is_enabled(void)
{
	return false;
}

static inline void calcurr_set_enabled(bool enable)
{
}

#endif /* CONF_CALIB */
#endif /* CALIB_CURRENT_H_ */
