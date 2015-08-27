#ifndef SETTINGS_H_
#define SETTINGS_H_

#include "util.h"
#include "fixpt.h"


struct settings {
	fixpt_t temp_setpoint;
	uint8_t reserved[13];
	uint8_t serial;
} _packed;


struct settings * get_settings(void);
void store_settings(void);

void settings_work(void);

void settings_init(void);

#endif /* SETTINGS_H_ */
