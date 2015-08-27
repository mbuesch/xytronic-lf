#ifndef DISPLAY_H_
#define DISPLAY_H_

#include "util.h"


void display_force_dp(int8_t dp, bool force, bool enable);
void display_show(const char *digits);

void display_enable(bool enable);

void display_work(void);
void display_init(void);

#endif /* DISPLAY_H_ */
