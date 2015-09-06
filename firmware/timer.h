#ifndef TIMER_H_
#define TIMER_H_

#include "util.h"


#ifndef TIMER_CPS
# define TIMER_CPS	1000
#endif

#define TIMERCPS	((_timcnt_t)(TIMER_CPS))

#ifndef TIMER_BITLEN
# define TIMER_BITLEN	16
#endif

#if TIMER_BITLEN == 16
typedef uint16_t _timcnt_t;
typedef int16_t _signed_timcnt_t;
#elif TIMER_BITLEN == 24
typedef uint24_t _timcnt_t;
typedef int24_t _signed_timcnt_t;
#elif TIMER_BITLEN == 32
typedef uint32_t _timcnt_t;
typedef int32_t _signed_timcnt_t;
#else
# error "Invalid TIMER_BITLEN"
#endif

struct timer {
	_timcnt_t count;
};


_timcnt_t _timer_get_now(void);

static inline _signed_timcnt_t _timer_ms_to_count(int32_t millisec)
{
	return (_signed_timcnt_t)sdiv_round_up((int32_t)TIMERCPS * millisec,
					       (int32_t)1000);
}

static inline int32_t _timer_count_to_ms(_signed_timcnt_t count)
{
	return sdiv_round((int32_t)count * (int32_t)1000,
			  (int32_t)TIMERCPS);
}

static inline _signed_timcnt_t _timer_count_since(const struct timer *timer)
{
	return (_signed_timcnt_t)(_timer_get_now() - timer->count);
}

static inline int32_t timer_ms_since(const struct timer *timer)
{
	return _timer_count_to_ms(_timer_count_since(timer));
}

bool timer_expired(const struct timer *timer);

static inline void timer_arm(struct timer *timer, int32_t millisec)
{
	timer->count = _timer_get_now() + (_timcnt_t)_timer_ms_to_count(millisec);
}

static inline void timer_set_now(struct timer *timer)
{
	timer->count = _timer_get_now();
}

static inline void timer_add(struct timer *timer, int32_t millisec)
{
	timer->count += (_timcnt_t)_timer_ms_to_count(millisec);
}

void timer_init(void);

#endif /* TIMER_H_ */
