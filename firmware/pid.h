#ifndef PID_H_
#define PID_H_

#include "fixpt.h"


struct pid {
	fixpt_t kp;
	fixpt_t ki;
	fixpt_t kd;
	fixpt_t d_decay_div;

	fixpt_t setpoint;
	fixpt_t y_neglim;
	fixpt_t y_poslim;

	fixpt_t prev_e;
	fixpt_t integr;
};

void pid_reset(struct pid *pid);

void pid_set_factors(struct pid *pid,
		     fixpt_t kp, fixpt_t ki, fixpt_t kd);

static inline void pid_set_d_decay_div(struct pid *pid, fixpt_t decay_div)
{
	pid->d_decay_div = decay_div;
}

void pid_init(struct pid *pid,
	      fixpt_t kp, fixpt_t ki, fixpt_t kd,
	      fixpt_t y_neglim, fixpt_t y_poslim);

static inline void pid_set_setpoint(struct pid *pid, fixpt_t setpoint)
{
	pid->setpoint = setpoint;
}

static inline fixpt_t pid_get_setpoint(struct pid *pid)
{
	return pid->setpoint;
}

fixpt_t pid_run(struct pid *pid, fixpt_t dt, fixpt_t r);

#endif /* PID_H_ */
