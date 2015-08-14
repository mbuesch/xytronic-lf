#include "test.h"
#include "timer.c"


static void check_timer(void)
{
	struct timer t;

	CHECK(TIMER_CPS == 100);

	/* Check: init */
	timer_init();
	CHECK(TIMSK0 & (1 << OCIE0A));
	CHECK(TIFR0 == (1 << OCF0A));
	CHECK(TCCR0A & (1 << WGM01));
	CHECK((TCCR0B & ((1 << CS02) | (1 << CS01) | (1 << CS00))) ==
			((1 << CS02) | (1 << CS01) | (1 << CS00)));
	CHECK(abs((long)F_CPU - (OCR0A * 1024 * TIMER_CPS)) < ((long)F_CPU / 100));


	/* Check: ISR */
	_timer_count_now = 42;
	TIMER0_COMPA_vect();
	CHECK_EQ_UU(_timer_count_now, 43);


	/* Check: get */
	_timer_count_now = 42;
	CHECK_EQ_UU(_timer_get_now(), 42);


	/* Check: ms to count */
	CHECK_EQ_II(_timer_ms_to_count(50),
		    TIMER_CPS * 50 / 1000);
	CHECK_EQ_II(_timer_ms_to_count(100),
		    TIMER_CPS * 100 / 1000);
	CHECK_EQ_II(_timer_ms_to_count(1000),
		    TIMER_CPS * 1000 / 1000);
	CHECK_EQ_II(_timer_ms_to_count(2000),
		    TIMER_CPS * 2000 / 1000);
	/* Rounded (round up) */
	CHECK_EQ_II(_timer_ms_to_count(1), 1);
	CHECK_EQ_II(_timer_ms_to_count(2), 1);
	/* Negative */
	CHECK_EQ_II(_timer_ms_to_count(-50),
		    (_signed_timcnt_t)(TIMER_CPS * -50 / 1000));
	CHECK_EQ_II(_timer_ms_to_count(-100),
		    (_signed_timcnt_t)(TIMER_CPS * -100 / 1000));
	CHECK_EQ_II(_timer_ms_to_count(-1000),
		    (_signed_timcnt_t)(TIMER_CPS * -1000 / 1000));
	CHECK_EQ_II(_timer_ms_to_count(-2000),
		    (_signed_timcnt_t)(TIMER_CPS * -2000 / 1000));
	/* Rounded (round up) */
	CHECK_EQ_II(_timer_ms_to_count(-1), 0);
	CHECK_EQ_II(_timer_ms_to_count(-2), 0);
	CHECK_EQ_II(_timer_ms_to_count(-9), 0);
	CHECK_EQ_II(_timer_ms_to_count(-10), -1);


	/* Check: count to ms */
	CHECK_EQ_II(_timer_count_to_ms((_signed_timcnt_t)0),
		    (int32_t)(1000 * 0 / TIMER_CPS));
	CHECK_EQ_II(_timer_count_to_ms((_signed_timcnt_t)1),
		    (int32_t)(1000 * 1 / TIMER_CPS));
	CHECK_EQ_II(_timer_count_to_ms((_signed_timcnt_t)50),
		    (int32_t)(1000 * 50 / TIMER_CPS));
	CHECK_EQ_II(_timer_count_to_ms((_signed_timcnt_t)100),
		    (int32_t)(1000 * 100 / TIMER_CPS));
	CHECK_EQ_II(_timer_count_to_ms((_signed_timcnt_t)10000),
		    (int32_t)(1000 * 10000 / TIMER_CPS));
	CHECK_EQ_II(_timer_count_to_ms((_signed_timcnt_t)32767),
		    (int32_t)(1000 * 32767 / TIMER_CPS));
	/* negative */
	CHECK_EQ_II(_timer_count_to_ms((_signed_timcnt_t)-1),
		    (int32_t)(1000 * -1 / TIMER_CPS));
	CHECK_EQ_II(_timer_count_to_ms((_signed_timcnt_t)-50),
		    (int32_t)(1000 * -50 / TIMER_CPS));
	CHECK_EQ_II(_timer_count_to_ms((_signed_timcnt_t)-100),
		    (int32_t)(1000 * -100 / TIMER_CPS));
	CHECK_EQ_II(_timer_count_to_ms((_signed_timcnt_t)-10000),
		    (int32_t)(1000 * -10000 / TIMER_CPS));
	CHECK_EQ_II(_timer_count_to_ms((_signed_timcnt_t)-32768),
		    (int32_t)(1000 * -32768 / TIMER_CPS));


	/* Check: arm */
	_timer_count_now = 42;
	timer_arm(&t, 100);
	CHECK_EQ_UU(t.count,
		    42u + (uint32_t)_timer_ms_to_count(100));


	/* Check: add */
	_timer_count_now = 42;
	timer_arm(&t, 100);
	timer_add(&t, 50);
	CHECK_EQ_UU(t.count,
		    42u + (uint32_t)_timer_ms_to_count(100) +
			  (uint32_t)_timer_ms_to_count(50));


	/* Check: count since */
	_timer_count_now = 42;
	timer_arm(&t, 0);
	t.count = 42;
	CHECK_EQ_II(_timer_count_since(&t),
		    (_signed_timcnt_t)0);
	t.count = 43;
	CHECK_EQ_II(_timer_count_since(&t),
		    (_signed_timcnt_t)-1);
	t.count = 41;
	CHECK_EQ_II(_timer_count_since(&t),
		    (_signed_timcnt_t)1);
	t.count = (_timcnt_t)-42;
	CHECK_EQ_II(_timer_count_since(&t),
		    (_signed_timcnt_t)84);


	/* Check: ms since */
	_timer_count_now = (_timcnt_t)_timer_ms_to_count(500);
	timer_arm(&t, 0);
	t.count = (_timcnt_t)_timer_ms_to_count(500);
	CHECK_EQ_II(timer_ms_since(&t), 0);
	t.count = (_timcnt_t)_timer_ms_to_count(600);
	CHECK_EQ_II(timer_ms_since(&t), -100);
	t.count = (_timcnt_t)_timer_ms_to_count(400);
	CHECK_EQ_II(timer_ms_since(&t), 100);
	t.count = (_timcnt_t)_timer_ms_to_count(-500);
	CHECK_EQ_II(timer_ms_since(&t), 1000);


	/* Check: expired */
	_timer_count_now = (_timcnt_t)_timer_ms_to_count(500);
	timer_arm(&t, 0);
	t.count = (_timcnt_t)_timer_ms_to_count(500);
	CHECK(timer_expired(&t));
	t.count = (_timcnt_t)_timer_ms_to_count(600);
	CHECK(!timer_expired(&t));
	t.count = (_timcnt_t)_timer_ms_to_count(400);
	CHECK(timer_expired(&t));
	t.count = (_timcnt_t)_timer_ms_to_count(-500);
	CHECK(timer_expired(&t));
}

int main(void)
{
	check_timer();

	return 0;
}
