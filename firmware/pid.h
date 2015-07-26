#ifndef PID_H_
#define PID_H_

#include <stdint.h>


/* Q10.5 */
#define PIDVAL_SIZE	16
#define PIDVAL_SHIFT	5

#define PIDFLOAT_SIZE	32


#if PIDVAL_SIZE == 8
typedef int8_t pidval_t;
#elif PIDVAL_SIZE == 16
typedef int16_t pidval_t;
#elif PIDVAL_SIZE == 32
typedef int32_t pidval_t;
#else
# error "Invalid PIDVAL_SIZE"
#endif

#if PIDFLOAT_SIZE == 32
typedef float pidfloat_t;
#elif PIDFLOAT_SIZE == 64
typedef double pidfloat_t;
#else
# error "Invalid PIDFLOAT_SIZE"
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

static inline pidval_t int_to_pidval(int32_t i)
{
	return (pidval_t)(i * (1L << PIDVAL_SHIFT));
}

static inline int32_t pidval_to_int(pidval_t p)
{
	return (int32_t)((p + (1L << (PIDVAL_SHIFT - 1))) / (1L << PIDVAL_SHIFT));
}

static inline pidval_t float_to_pidval(pidfloat_t f)
{
	if (f < 0)
		return (pidval_t)((f * (1L << PIDVAL_SHIFT)) - 0.5f);
	else
		return (pidval_t)((f * (1L << PIDVAL_SHIFT)) + 0.5f);
}

static inline pidfloat_t pidval_to_float(pidval_t p)
{
	return (pidfloat_t)p / (pidfloat_t)(1L << PIDVAL_SHIFT);
}

void pid_init(struct pid *pid,
	      pidval_t kp, pidval_t ki, pidval_t kd,
	      pidval_t y_lim);

static inline void pid_set_setpoint(struct pid *pid, pidval_t setpoint)
{
	pid->setpoint = setpoint;
	pid->prev_e = int_to_pidval(0);
	pid->integr = int_to_pidval(0);
}

static inline pidval_t pid_get_setpoint(struct pid *pid)
{
	return pid->setpoint;
}

pidval_t pid_run(struct pid *pid, pidval_t dt, pidval_t r);

#endif /* PID_H_ */
