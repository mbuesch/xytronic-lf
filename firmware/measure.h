#ifndef MEASURE_H_
#define MEASURE_H_

#include "fixpt.h"
#include "util.h"


/* Multiplexer selections */
#define MEAS_MUX_ADC0		((0 << MUX3) | (0 << MUX2) | (0 << MUX1) | (0 << MUX0))
#define MEAS_MUX_ADC1		((0 << MUX3) | (0 << MUX2) | (0 << MUX1) | (1 << MUX0))
#define MEAS_MUX_ADC2		((0 << MUX3) | (0 << MUX2) | (1 << MUX1) | (0 << MUX0))
#define MEAS_MUX_ADC3		((0 << MUX3) | (0 << MUX2) | (1 << MUX1) | (1 << MUX0))
#define MEAS_MUX_ADC4		((0 << MUX3) | (1 << MUX2) | (0 << MUX1) | (0 << MUX0))
#define MEAS_MUX_ADC5		((0 << MUX3) | (1 << MUX2) | (0 << MUX1) | (1 << MUX0))
#define MEAS_MUX_ADC6		((0 << MUX3) | (1 << MUX2) | (1 << MUX1) | (0 << MUX0))
#define MEAS_MUX_ADC7		((0 << MUX3) | (1 << MUX2) | (1 << MUX1) | (1 << MUX0))
#define MEAS_MUX_ADC8		((1 << MUX3) | (0 << MUX2) | (0 << MUX1) | (0 << MUX0))
#define MEAS_MUX_BG		((1 << MUX3) | (1 << MUX2) | (1 << MUX1) | (0 << MUX0))
#define MEAS_MUX_GND		((1 << MUX3) | (1 << MUX2) | (1 << MUX1) | (1 << MUX0))

/* Prescaler selections */
#define MEAS_PS_2		((0 << ADPS2) | (0 << ADPS1) | (1 << ADPS0))
#define MEAS_PS_4		((0 << ADPS2) | (1 << ADPS1) | (0 << ADPS0))
#define MEAS_PS_8		((0 << ADPS2) | (1 << ADPS1) | (1 << ADPS0))
#define MEAS_PS_16		((1 << ADPS2) | (0 << ADPS1) | (0 << ADPS0))
#define MEAS_PS_32		((1 << ADPS2) | (0 << ADPS1) | (1 << ADPS0))
#define MEAS_PS_64		((1 << ADPS2) | (1 << ADPS1) | (0 << ADPS0))
#define MEAS_PS_128		((1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0))

/* Reference selection */
#define MEAS_REF_AREF		((0 << REFS1) | (0 << REFS0))
#define MEAS_REF_AVCC		((0 << REFS1) | (1 << REFS0))
#define MEAS_REF_RESERVED	((1 << REFS1) | (0 << REFS0))
#define MEAS_REF_INT1P1V	((1 << REFS1) | (1 << REFS0))

#define MEASURE_MAX_RESULT	0x3FF

enum measure_chan {
	MEAS_CHAN_0,
	MEAS_CHAN_1,

	NR_MEAS_CHANS
};

enum measure_plausibility {
	MEAS_PLAUSIBLE,
	MEAS_NOT_PLAUSIBLE,
	MEAS_PLAUS_TIMEOUT,
};

/* Callback runs in IRQ context. */
typedef void (*measure_cb_t)(fixpt_t measured_phys_value,
			     enum measure_plausibility plaus);

struct measure_config {
	char name[3];

	uint8_t mux;
	uint8_t ps;
	uint8_t ref;

//FIXME: Do this via time instead?
	uint16_t averaging_count;

	uint16_t scale_raw_lo;
	uint16_t scale_raw_hi;
	fixpt_t scale_phys_lo;
	fixpt_t scale_phys_hi;

	fixpt_t plaus_neglim;
	fixpt_t plaus_poslim;
	uint16_t plaus_timeout_ms;

	measure_cb_t callback;
};


void measure_register_channel(enum measure_chan chan,
			      const struct measure_config __flash *config);

void measure_start(void);
void measure_work(void);
void measure_init(void);

#endif /* MEASURE_H_ */
