#ifndef CONTROLLER_TEMP_H_
#define CONTROLLER_TEMP_H_

#include "fixpt.h"


#define CONTRTEMP_NEGLIM	0.0
#define CONTRTEMP_POSLIM	480.0


void contrtemp_set_feedback(fixpt_t r);
void contrtemp_set_setpoint(fixpt_t w);

void contrtemp_work(void);
void contrtemp_init(void);

#endif /* CONTROLLER_TEMP_H_ */
