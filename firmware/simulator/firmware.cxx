#include "../bitmap.c"
#include "../buttons.c"
#include "../controller_current.c"
#include "../controller_temp.c"
#include "../debug_uart.c"
#include "../display.c"
#include "../filter.c"
#include "../fixpt.c"
#include "../measure.c"
#include "../measure_current.c"
#include "../measure_temp.c"
#include "../menu.c"
#include "../pid.c"
#include "../presets.c"
#include "../pwm_current.c"
#include "../ring.c"
#include "../scale.c"
#include "../settings.c"
#include "../sseg.c"
#include "../timer.c"
#include "../util.c"

#define RUN_MAIN_LOOP	0
#define main		program_main
#include "../main.c"
#undef main

#include "../tests/fake/fake.c"
