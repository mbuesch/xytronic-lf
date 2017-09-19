#ifndef PRESETS_H_
#define PRESETS_H_

#include "fixpt.h"


#if CONF_PRESETS

/* Presets feature ENABLED */

/* The global number of presets available. */
#define NR_PRESETS		6

/* Preset default values. */
#define PRESET_DEFAULT0		CELSIUS(305)
#define PRESET_DEFAULT1		CELSIUS(320)
#define PRESET_DEFAULT2		CELSIUS(335)
#define PRESET_DEFAULT3		CELSIUS(350)
#define PRESET_DEFAULT4		CELSIUS(365)
#define PRESET_DEFAULT5		CELSIUS(380)
#define PRESET_DEFAULT_INDEX	3

#else /* CONF_PRESETS */

/* Presets feature DISABLED */

#define NR_PRESETS		1
#define PRESET_DEFAULT0		CELSIUS(350)
#define PRESET_DEFAULT_INDEX	0

#endif /* CONF_PRESETS */


void presets_next(void);
void presets_prev(void);
uint8_t presets_get_active_index(void);
fixpt_t presets_get_active_value(void);
void presets_set_active_value(fixpt_t value);

void presets_init(void);


#endif /* PRESETS_H_ */
