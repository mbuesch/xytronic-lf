#ifndef TIMER_H_
#define TIMER_H_

#include "util.h"


typedef uint32_t _timcnt_t;
typedef int32_t _signed_timcnt_t;

#define TIMER_CPS	((_timcnt_t)100)

extern _timcnt_t _timer_count_now;

struct timer {
	_timcnt_t count;
};

static inline _timcnt_t _timer_ms_to_count(uint32_t millisec)
{
	return (_timcnt_t)div_round_up(TIMER_CPS * millisec, 1000UL);
}

static inline _timcnt_t _timer_get_now(void)
{
	_timcnt_t count;
	uint8_t sreg;

	sreg = irq_disable_save();
	count = _timer_count_now;
	irq_restore(sreg);

	return count;
}

static inline bool timer_expired(const struct timer *timer)
{
	return (_signed_timcnt_t)(_timer_get_now() - timer->count) >= 0;
}

static inline void timer_arm(struct timer *timer, uint32_t millisec)
{
	timer->count = _timer_get_now() + _timer_ms_to_count(millisec);
}

static inline void timer_add(struct timer *timer, uint32_t millisec)
{
	timer->count += _timer_ms_to_count(millisec);
}

void timer_init(void);

#endif /* TIMER_H_ */
