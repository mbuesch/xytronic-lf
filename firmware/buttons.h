#ifndef BUTTONS_H_
#define BUTTONS_H_

#include "util.h"


enum button_id {
	BUTTON_SET,
	BUTTON_MINUS,
	BUTTON_PLUS,

	NR_BUTTONS,
};

typedef void (*button_handler_t)(enum button_id button, bool pos_edge);

void buttons_register_handler(enum button_id button,
			      button_handler_t handler);

bool buttons_debug_requested(void);

void buttons_work(void);
void buttons_init(void);

#endif /* BUTTONS_H_ */