#ifndef CONTROLLER_CURRENT_H_
#define CONTROLLER_CURRENT_H_

#include "util.h"
#include "fixpt.h"


#define AMPERE(ampere)			((float)(ampere) * 10.0) /* Convert to 1/10th amps */

/* Current controller PID parameters */
#define CONTRCURR_PID_KP		1.0
#define CONTRCURR_PID_KI		0.3
#define CONTRCURR_PID_KD		0.0
#define CONTRCURR_PID_D_DECAY		1.0

/* Current controller integral part limits. */
#define CONTRCURR_NEGLIM_I		AMPERE(-2.5)
#define CONTRCURR_POSLIM_I		AMPERE(2.5)
/* Current controller final limits. */
#define CONTRCURR_NEGLIM		AMPERE(0)
#define CONTRCURR_POSLIM		AMPERE(5)

/* Current restriction on low temperatures. */
#define CONTRCURR_RESTRICT_TOTEMP	CELSIUS(100)
#define CONTRCURR_RESTRICT_MAXCURR	AMPERE(2)

/* PID cut off current. PID is only active below this setpoint. */
#define CONTRCURR_PID_CUTOFF_HYST	(AMPERE(CONF_CURRCUTOFFHYST))
#define CONTRCURR_PID_CUTOFF_HI		(AMPERE(CONF_CURRCUTOFF))
#define CONTRCURR_PID_CUTOFF_LO		(CONTRCURR_PID_CUTOFF_HI - CONTRCURR_PID_CUTOFF_HYST)


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
