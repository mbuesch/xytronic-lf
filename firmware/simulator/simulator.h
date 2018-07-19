#ifndef SIMULATOR_H_
#define SIMULATOR_H_

#include <stdint.h>


void sim_irq_disable(void);
void sim_irq_enable(void);
uint8_t sim_irq_disable_save(void);
void sim_irq_restore(uint8_t sreg_flags);

size_t simulator_uart_get_tx(uint8_t *buf, size_t max_bytes);
bool simulator_pwm_get(int pwm_index, uint16_t *value, uint16_t *max_value);
bool simulator_adc_set(int adc_index, uint16_t value);

void simulator_mainloop_once(void);
void simulator_exit(void);
bool simulator_init(void);

#endif /* SIMULATOR_H_ */
