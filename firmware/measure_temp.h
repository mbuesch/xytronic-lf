#ifndef MEASURE_TEMP_H_
#define MEASURE_TEMP_H_

#include "fixpt.h"


void meastemp_adjust_set(fixpt_t adjustment);
fixpt_t meastemp_adjust_get(void);

void meastemp_init(void);

#endif /* MEASURE_TEMP_H_ */
