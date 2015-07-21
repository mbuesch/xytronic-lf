#ifndef PID_H_
#define PID_H_

#include <stdint.h>


/* Q10.5 */
#define PIDVAL_SIZE	16
#define PIDVAL_SHIFT	5


#if PIDVAL_SIZE == 8
typedef int8_t pidval_t;
#elif PIDVAL_SIZE == 16
typedef int16_t pidval_t;
#elif PIDVAL_SIZE == 32
typedef int32_t pidval_t;
#else
# error "Invalid PIDVAL_SIZE"
#endif

struct pid {
	pidval_t kp;
	pidval_t ki;
	pidval_t kd;

	pidval_t setpoint;
	pidval_t y_lim;

	pidval_t prev_e;
	pidval_t integr;
};

#define INT_TO_PIDVAL(i)	((pidval_t)((i) * (1 << PIDVAL_SHIFT)))
#define PIDVAL_TO_INT(p)	((int32_t)(((p) + (1 << (PIDVAL_SHIFT - 1))) / (1 << PIDVAL_SHIFT)))

#define FLOAT_TO_PIDVAL(f)	((pidval_t)(((f) * (1 << PIDVAL_SHIFT)) + 0.5f))
#define PIDVAL_TO_FLOAT(p)	(((float)(p)) / (1 << PIDVAL_SHIFT))

void pid_init(struct pid *pid,
	      pidval_t kp, pidval_t ki, pidval_t kd,
	      pidval_t y_lim);

static inline void pid_set_setpoint(struct pid *pid, pidval_t setpoint)
{
	pid->setpoint = setpoint;
	pid->prev_e = INT_TO_PIDVAL(0);
	pid->integr = INT_TO_PIDVAL(0);
}

static inline pidval_t pid_get_setpoint(struct pid *pid)
{
	return pid->setpoint;
}

pidval_t pid_run(struct pid *pid, pidval_t dt, pidval_t r);

#endif /* PID_H_ */
