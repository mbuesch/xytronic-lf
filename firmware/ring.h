#ifndef RING_H_
#define RING_H_

#include "util.h"


uint8_t ring_next(uint8_t current, uint8_t max_index);
uint8_t ring_prev(uint8_t current, uint8_t max_index);


#endif /* RING_H_ */
