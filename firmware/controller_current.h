#ifndef CONTROLLER_CURRENT_H_
#define CONTROLLER_CURRENT_H_

#include "util.h"
#include "fixpt.h"


#define CONTRCURR_NEGLIM	0.0
#define CONTRCURR_POSLIM	5.0


void contrcurr_set_feedback(fixpt_t r);
void contrcurr_set_setpoint(fixpt_t w);

void contrcurr_set_enabled(bool enable,
			   uint8_t disabled_curr_percent);
void contrcurr_set_emerg(bool emergency);

void contrcurr_work(void);
void contrcurr_init(void);

#endif /* CONTROLLER_CURRENT_H_ */
