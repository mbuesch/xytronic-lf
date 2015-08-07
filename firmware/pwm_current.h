#ifndef PWM_CURRENT_H_
#define PWM_CURRENT_H_

#include "fixpt.h"


void pwmcurr_set(fixpt_t current_amps);

void pwmcurr_work(void);
void pwmcurr_init(void);

#endif /* PWM_CURRENT_H_ */
