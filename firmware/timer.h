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


bool timer_expired(const struct timer *timer);
int32_t timer_ms_since(const struct timer *timer);

void timer_arm(struct timer *timer, int32_t millisec);
void timer_set_now(struct timer *timer);
void timer_add(struct timer *timer, int32_t millisec);

void timer_init(void);

#endif /* TIMER_H_ */
