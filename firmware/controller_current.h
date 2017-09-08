#ifndef CONTROLLER_CURRENT_H_
#define CONTROLLER_CURRENT_H_

#include "util.h"
#include "fixpt.h"


#define AMPERE(ampere)			((float)(ampere) * 10.0) /* Convert to 1/10th amps */

#define CONTRCURR_NEGLIM		AMPERE(0)
#define CONTRCURR_POSLIM		AMPERE(5)

#define CONTRCURR_RESTRICT_TOTEMP	100.0
#define CONTRCURR_RESTRICT_MAXCURR	AMPERE(2)


void contrcurr_set_feedback(fixpt_t r);
void contrcurr_set_setpoint(fixpt_t w);

void contrcurr_set_restricted(bool restricted);
void contrcurr_set_enabled(bool enable,
			   uint8_t disabled_curr_percent);

enum contrcurr_emerg_flags {
	CONTRCURR_EMERG_UNPLAUS_FEEDBACK	= 1 << 0,
	CONTRCURR_EMERG_HIGH_TEMP		= 1 << 1,
};

void contrcurr_set_emerg(uint8_t emergency_flags);
uint8_t contrcurr_get_emerg(void);

void contrcurr_init(void);

#endif /* CONTROLLER_CURRENT_H_ */
