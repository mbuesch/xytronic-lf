#ifndef CONTROLLER_TEMP_H_
#define CONTROLLER_TEMP_H_

#include "util.h"
#include "fixpt.h"


/* Temperature controller PID parameters */
#define CONTRTEMP_PID_KP_NORMAL		4.0
#define CONTRTEMP_PID_KI_NORMAL		0.04
#define CONTRTEMP_PID_KD_NORMAL		0.5
#define CONTRTEMP_PID_D_DECAY_NORMAL	1.2

/* Temperature controller boost (1) PID parameters */
#if CONF_BOOST
# define CONTRTEMP_PID_KP_BOOST1	6.0
# define CONTRTEMP_PID_KI_BOOST1	0.08
# define CONTRTEMP_PID_KD_BOOST1	1.0
# define CONTRTEMP_PID_D_DECAY_BOOST1	1.5
#endif

/* Temperature controller boost (2) PID parameters */
#if CONF_BOOST
# define CONTRTEMP_PID_KP_BOOST2	12.0
# define CONTRTEMP_PID_KI_BOOST2	0.1
# define CONTRTEMP_PID_KD_BOOST2	1.5
# define CONTRTEMP_PID_D_DECAY_BOOST2	1.5
#endif

/* Temperature controller limits. */
#define CONTRTEMP_NEGLIM		0.0
#define CONTRTEMP_POSLIM		480.0

/* Temperature to current mapping. */
#define CONTRTEMP_MAP_TEMPLO		0.0
#define CONTRTEMP_MAP_CURRLO		0.0
#define CONTRTEMP_MAP_TEMPHI		180.0
#define CONTRTEMP_MAP_CURRHI		5.0

/* Temperature controller default idle setpoint. */
#define CONTRTEMP_DEF_IDLE_SETPOINT	150.0


enum contrtemp_boostmode {
	TEMPBOOST_NORMAL,
#if CONF_BOOST
	TEMPBOOST_BOOST1,
	TEMPBOOST_BOOST2,
#endif

	NR_BOOST_MODES,
};


void contrtemp_set_feedback(fixpt_t r);
fixpt_t contrtemp_get_feedback(void);

void contrtemp_update_setpoint(void);
void contrtemp_set_idle_setpoint(fixpt_t w);

void contrtemp_set_enabled(bool enabled);
void contrtemp_set_emerg(bool emergency);
bool contrtemp_in_emerg(void);
bool contrtemp_is_idle(void);
bool contrtemp_is_heating_up(void);
enum contrtemp_boostmode contrtemp_get_boost_mode(void);
void contrtemp_update_pid_config(void);

void contrtemp_init(void);

#endif /* CONTROLLER_TEMP_H_ */
