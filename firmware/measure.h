#ifndef MEASURE_H_
#define MEASURE_H_

#include "util.h"


enum measure_chan {
	MEASCHAN_ADC0,
	MEASCHAN_ADC1,
	MEASCHAN_ADC2,
	MEASCHAN_ADC3,
	MEASCHAN_ADC4,
	MEASCHAN_ADC5,
};

#define MEASURE_MAX_RESULT	0x3FF


/* Callback runs in IRQ context. */
typedef void (*measure_cb_t)(void *context, uint16_t raw_adc);

bool measure_schedule(enum measure_chan chan,
		      measure_cb_t callback, void *context);

void measure_init(void);

#endif /* MEASURE_H_ */
