#ifndef FAKE_DELAY_H_
#define FAKE_DELAY_H_

#include <time.h>
#include <errno.h>
#include <string.h>
#include <math.h>
#include <stdio.h>

static inline void _msleep_(unsigned int msecs)
{
	int err;
	struct timespec time;

	time.tv_sec = 0;
	while (msecs >= 1000) {
		time.tv_sec++;
		msecs -= 1000;
	}
	time.tv_nsec = msecs;
	time.tv_nsec *= 1000000;
	do {
		err = nanosleep(&time, &time);
	} while (err && errno == EINTR);
	if (err) {
		fprintf(stderr, "nanosleep() failed with: %s\n",
			strerror(errno));
	}
}

static inline void _delay_ms(double ms)
{
	_msleep_((unsigned int)ceil(ms));
}

static inline void _delay_us(double us)
{
	_msleep_(1);
}

#endif /* FAKE_DELAY_H_ */
