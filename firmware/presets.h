#ifndef PRESETS_H_
#define PRESETS_H_

#include "fixpt.h"


#if CONF_PRESETS

/* The global number of presets available. */
#define NR_PRESETS		3

/* Preset default values. */
#define PRESET_DEFAULT0		330.0
#define PRESET_DEFAULT1		350.0
#define PRESET_DEFAULT2		370.0
#define PRESET_DEFAULT_INDEX	1

#else /* CONF_PRESETS */

#define NR_PRESETS		1

#define PRESET_DEFAULT0		350.0
#define PRESET_DEFAULT_INDEX	0

#endif /* CONF_PRESETS */


void presets_next(void);
void presets_prev(void);
uint8_t presets_get_active_index(void);
fixpt_t presets_get_active_value(void);
void presets_set_active_value(fixpt_t value);

void presets_init(void);


#endif /* PRESETS_H_ */
