#ifndef CONTROLLER_TEMP_H_
#define CONTROLLER_TEMP_H_

#include "util.h"
#include "fixpt.h"


#define CONTRTEMP_NEGLIM	0.0
#define CONTRTEMP_POSLIM	480.0


void contrtemp_set_feedback(fixpt_t r);
fixpt_t contrtemp_get_feedback(void);

void contrtemp_set_setpoint(fixpt_t w);
fixpt_t contrtemp_get_setpoint(void);

void contrtemp_work(void);
void contrtemp_init(void);

#endif /* CONTROLLER_TEMP_H_ */
