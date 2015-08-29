#ifndef BUTTONS_H_
#define BUTTONS_H_

#include "util.h"


enum button_id {
	BUTTON_SET,
	BUTTON_MINUS,
	BUTTON_PLUS,
	//TODO iron button

	NR_BUTTONS,
};

enum button_state {
	BSTATE_POSEDGE,
	BSTATE_NEGEDGE,
	BSTATE_PRESSED,
};

typedef void (*button_handler_t)(enum button_id button,
				 enum button_state state);

void buttons_register_handler(enum button_id button,
			      button_handler_t handler);

uint8_t button_is_pressed(enum button_id button);

void buttons_work(void);
void buttons_init(void);

#endif /* BUTTONS_H_ */
