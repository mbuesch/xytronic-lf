#ifndef PID_H_
#define PID_H_

#include "fixpt.h"


struct pid_k_set {
	fixpt_t kp;
	fixpt_t ki;
	fixpt_t kd;
	fixpt_t d_decay_div;
};

struct pid {
	struct pid_k_set k;

	fixpt_t setpoint;

	fixpt_t i_neglim;
	fixpt_t i_poslim;

	fixpt_t y_neglim;
	fixpt_t y_poslim;

	fixpt_t prev_e;
	fixpt_t integr;

#if CONF_DEBUG
	const char __flash *name;
	fixpt_t debug_old_e;
	fixpt_t debug_old_p;
	fixpt_t debug_old_i;
	fixpt_t debug_old_d;
	fixpt_t debug_old_pe;
#endif
};

void pid_reset(struct pid *pid);

void pid_set_factors(struct pid *pid, const struct pid_k_set *k);

void pid_init(struct pid *pid,
#if CONF_DEBUG
	      const char __flash *name,
#endif
	      const struct pid_k_set *k,
	      fixpt_t i_neglim, fixpt_t i_poslim,
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
