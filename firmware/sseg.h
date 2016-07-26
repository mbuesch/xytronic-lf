#ifndef SEVENSEGMENT_H_
#define SEVENSEGMENT_H_

#include "util.h"


enum sseg_segment_id {
	SSEG_A,
	SSEG_B,
	SSEG_C,
	SSEG_D,
	SSEG_E,
	SSEG_F,
	SSEG_G,
	SSEG_DP,

	SSEG_NR_SEGMENTS,
};

struct sseg_segment_iomap {
	sfr_addr_t segment_ddr;
	sfr_addr_t segment_port;
	uint8_t segment_masks[SSEG_NR_SEGMENTS];
	bool segment_polarity;
};

struct sseg_iomap {
	const struct sseg_segment_iomap __flash *segment;

	sfr_addr_t mux_ddr;
	sfr_addr_t mux_port;
	uint8_t mux_mask;
	bool mux_polarity;
};

struct sseg_digit_data {
	const struct sseg_iomap __flash *iomap;
	uint8_t segment_enable_set;
	uint8_t segment_enable_mask;
};

#define SSEG_DIGIT_DP		((char)(1u << 7))

void sseg_digit_set(struct sseg_digit_data *ddata,
		    char digit);

void sseg_multiplex(const struct sseg_digit_data *ddata_from,
		    const struct sseg_digit_data *ddata_to);

void sseg_init(struct sseg_digit_data *ddata);
void sseg_exit(struct sseg_digit_data *ddata);

#endif /* SEVENSEGMENT_H_ */
