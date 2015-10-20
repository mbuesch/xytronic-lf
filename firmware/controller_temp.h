#ifndef CONTROLLER_TEMP_H_
#define CONTROLLER_TEMP_H_

#include "util.h"
#include "fixpt.h"


/* Temperature controller PID parameters */
#define CONTRTEMP_PID_KP_NORMAL		4.0
#define CONTRTEMP_PID_KI_NORMAL		0.08
#define CONTRTEMP_PID_KD_NORMAL		1.0
#define CONTRTEMP_PID_D_DECAY_NORMAL	1.2
/* Temperature controller boost (1) PID parameters */
#define CONTRTEMP_PID_KP_BOOST1		6.0
#define CONTRTEMP_PID_KI_BOOST1		0.08
#define CONTRTEMP_PID_KD_BOOST1		1.0
#define CONTRTEMP_PID_D_DECAY_BOOST1	1.5
/* Temperature controller boost (2) PID parameters */
#define CONTRTEMP_PID_KP_BOOST2		12.0
#define CONTRTEMP_PID_KI_BOOST2		0.1
#define CONTRTEMP_PID_KD_BOOST2		1.5
#define CONTRTEMP_PID_D_DECAY_BOOST2	1.5


#define CONTRTEMP_NEGLIM	0.0
#define CONTRTEMP_POSLIM	480.0


enum contrtemp_boostmode {
	TEMPBOOST_NORMAL,
	TEMPBOOST_BOOST1,
	TEMPBOOST_BOOST2,

	NR_BOOST_MODES,
};


void contrtemp_set_feedback(fixpt_t r);
fixpt_t contrtemp_get_feedback(void);

void contrtemp_set_setpoint(fixpt_t w);
fixpt_t contrtemp_get_setpoint(void);

void contrtemp_set_enabled(bool enabled);
void contrtemp_set_emerg(bool emergency);
bool contrtemp_in_emerg(void);
bool contrtemp_is_heating_up(void);
enum contrtemp_boostmode contrtemp_get_boost_mode(void);

void contrtemp_init(void);

#endif /* CONTROLLER_TEMP_H_ */
