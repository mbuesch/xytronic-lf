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

/* Digital-input-disable selections */
#define MEAS_DID_NONE		0u
#define MEAS_DID_ADC0		(1u << ADC0D)
#define MEAS_DID_ADC1		(1u << ADC1D)
#define MEAS_DID_ADC2		(1u << ADC2D)
#define MEAS_DID_ADC3		(1u << ADC3D)
#define MEAS_DID_ADC4		(1u << ADC4D)
#define MEAS_DID_ADC5		(1u << ADC5D)

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

#define MEASURE_MAX_RESULT	0x3FFu

enum measure_chan {
	MEAS_CHAN_CURRENT,
	MEAS_CHAN_TEMP,
};
#define NR_MEAS_CHANS	2

enum measure_plausibility {
	MEAS_PLAUSIBLE,
	MEAS_NOT_PLAUSIBLE,
	MEAS_PLAUS_TIMEOUT,
};

typedef void (*measure_result_cb_t)(fixpt_t measured_phys_value,
				    enum measure_plausibility plaus);
typedef uint16_t (*measure_filter_cb_t)(uint16_t raw_adc);

struct measure_config {
	/* A short two-character name. Just for debugging. */
	char name[3];

	/* The ADC multiplexer setting. One of MEAS_MUX_... */
	uint8_t mux;
	/* The digital-input-disable setting. One of MEAS_DID_... */
	uint8_t did;
	/* The ADC prescaler setting. One of MEAS_PS_... */
	uint8_t ps;
	/* The ADC reference setting. One of MEAS_REF_... */
	uint8_t ref;

	/* The time (in milliseconds) we keep adding samples for the
	 * averaging.
	 * The actual sample rate is: F_CPU / prescaler / 13
	 * The sum buffer is of size uint32_t.
	 */
	uint16_t averaging_timeout_ms;

	/* The scale() parameters for raw -> phys scaling. */
	uint16_t scale_raw_lo;
	uint16_t scale_raw_hi;
	fixpt_t scale_phys_lo;
	fixpt_t scale_phys_hi;

	/* The plausibility limits. */
	fixpt_t plaus_neglim;
	fixpt_t plaus_poslim;
	uint16_t plaus_timeout_ms;
};


void measure_adjust_set(enum measure_chan chan,
			fixpt_t adjustment);
fixpt_t measure_adjust_get(enum measure_chan chan);

void measure_start(void);
void measure_work(void);
void measure_init(void);

#endif /* MEASURE_H_ */
