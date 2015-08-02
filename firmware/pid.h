#ifndef PID_H_
#define PID_H_

#include "fixpt.h"


struct pid {
	fixpt_t kp;
	fixpt_t ki;
	fixpt_t kd;

	fixpt_t setpoint;
	fixpt_t y_lim;

	fixpt_t prev_e;
	fixpt_t integr;
};

void pid_init(struct pid *pid,
	      fixpt_t kp, fixpt_t ki, fixpt_t kd,
	      fixpt_t y_lim);

static inline void pid_set_setpoint(struct pid *pid, fixpt_t setpoint)
{
	pid->setpoint = setpoint;
	pid->prev_e = int_to_fixpt(0);
	pid->integr = int_to_fixpt(0);
}

static inline fixpt_t pid_get_setpoint(struct pid *pid)
{
	return pid->setpoint;
}

fixpt_t pid_run(struct pid *pid, fixpt_t dt, fixpt_t r);

#endif /* PID_H_ */
