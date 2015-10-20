#ifndef SETTINGS_H_
#define SETTINGS_H_

#include "util.h"
#include "fixpt.h"
#include "pid.h"
#include "controller_temp.h"


struct settings {
	fixpt_t temp_setpoint;
	struct pid_k_set temp_k[NR_BOOST_MODES];
	uint8_t reserved[24];
	uint8_t serial;
} _packed;


struct settings * get_settings(void);
void store_settings(void);

void settings_work(void);

void settings_init(void);

#endif /* SETTINGS_H_ */
