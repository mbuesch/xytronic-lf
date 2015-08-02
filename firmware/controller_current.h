#ifndef CONTROLLER_CURRENT_H_
#define CONTROLLER_CURRENT_H_

#include "fixpt.h"


fixpt_t contrcurr_get_controlvar(void);
void contrcurr_set_feedback(fixpt_t r);

void contrcurr_work(void);
void contrcurr_init(void);

#endif /* CONTROLLER_CURRENT_H_ */
