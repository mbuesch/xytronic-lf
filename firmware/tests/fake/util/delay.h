#ifndef FAKE_DELAY_H_
#define FAKE_DELAY_H_

#include <time.h>
#include <errno.h>
#include <string.h>
#include <math.h>
#include <stdio.h>


static inline void _usleep__(uint64_t us)
{
	int err;
	struct timespec tv;

	tv.tv_sec = 0;
	while (us >= 1000000u) {
		tv.tv_sec++;
		us -= 1000000u;
	}
	tv.tv_nsec = (int64_t)us * 1000;
	do {
		err = nanosleep(&tv, &tv);
	} while (err && errno == EINTR);
	if (err)
		perror("nanosleep()");
}

static inline void _delay_ms(double ms)
{
	_usleep__((uint64_t)ceil(ms * 1000.0));
}

static inline void _delay_us(double us)
{
	_usleep__((uint64_t)ceil(us));
}


#endif /* FAKE_DELAY_H_ */
