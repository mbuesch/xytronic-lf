#ifndef FAKE_IO_H_
#define FAKE_IO_H_

#include <endian.h>
#include <stdint.h>


#define _SFR_ADDR(x) (&(x))
#define _MMIO_BYTE(x) (*(uint8_t *)(x))
typedef volatile uint8_t * sfr_addr_t;
#define sfr_addr_t sfr_addr_t

extern volatile uint8_t MCUCSR;
#define MCUCSR	MCUCSR
#define MCUSR	MCUCSR

#define PB0	0
#define PB1	1
#define PB2	2
#define PB3	3
#define PB4	4
#define PB5	5
#define PB6	6
#define PB7	7
extern volatile uint8_t PORTB;
#define PORTB	PORTB
#define PC0	0
#define PC1	1
#define PC2	2
#define PC3	3
#define PC4	4
#define PC5	5
#define PC6	6
#define PC7	7
extern volatile uint8_t PORTC;
#define PORTC	PORTC
#define PD0	0
#define PD1	1
#define PD2	2
#define PD3	3
#define PD4	4
#define PD5	5
#define PD6	6
#define PD7	7
extern volatile uint8_t PORTD;
#define PORTD	PORTD
#define DDB0	0
#define DDB1	1
#define DDB2	2
#define DDB3	3
#define DDB4	4
#define DDB5	5
#define DDB6	6
#define DDB7	7
extern volatile uint8_t DDRB;
#define DDRB	DDRB
#define DDC0	0
#define DDC1	1
#define DDC2	2
#define DDC3	3
#define DDC4	4
#define DDC5	5
#define DDC6	6
#define DDC7	7
extern volatile uint8_t DDRC;
#define DDRC	DDRC
#define DDD0	0
#define DDD1	1
#define DDD2	2
#define DDD3	3
#define DDD4	4
#define DDD5	5
#define DDD6	6
#define DDD7	7
extern volatile uint8_t DDRD;
#define DDRD	DDRD
#define PINB0	0
#define PINB1	1
#define PINB2	2
#define PINB3	3
#define PINB4	4
#define PINB5	5
#define PINB6	6
#define PINB7	7
extern volatile uint8_t PINB;
#define PINB	PINB
#define PINC0	0
#define PINC1	1
#define PINC2	2
#define PINC3	3
#define PINC4	4
#define PINC5	5
#define PINC6	6
#define PINC7	7
extern volatile uint8_t PINC;
#define PINC	PINC
#define PIND0	0
#define PIND1	1
#define PIND2	2
#define PIND3	3
#define PIND4	4
#define PIND5	5
#define PIND6	6
#define PIND7	7
extern volatile uint8_t PIND;
#define PIND	PIND

#define OCIE0B	2
#define OCIE0A	1
#define TOIE0	0
extern volatile uint8_t TIMSK0;
#define TIMSK0	TIMSK0

#define OCF0B	2
#define OCF0A	1
#define TOV0	0
extern volatile uint8_t TIFR0;
#define TIFR0	TIFR0

#define ICIE1	5
#define OCIE1B	2
#define OCIE1A	1
#define TOIE1	0
extern volatile uint8_t TIMSK1;
#define TIMSK1	TIMSK1

#define ICF1	5
#define OCF1B	2
#define OCF1A	1
#define TOV1	0
extern volatile uint8_t TIFR1;
#define TIFR1	TIFR1

#define SREG_C	0
#define SREG_Z	1
#define SREG_N	2
#define SREG_V	3
#define SREG_S	4
#define SREG_H	5
#define SREG_T	6
#define SREG_I	7
extern volatile uint8_t SREG;
#define SREG	SREG

#define COM0A1	7
#define COM0A0	6
#define COM0B1	5
#define COM0B0	4
#define WGM01	1
#define WGM00	0
#define FOC0A	7
#define FOC0B	6
#define WGM02	3
#define CS02	2
#define CS01	1
#define CS00	0

#define COM1A1	7
#define COM1A0	6
#define COM1B1	5
#define COM1B0	4
#define WGM11	1
#define WGM10	0
#define ICNC1	7
#define ICES1	6
#define WGM13	4
#define WGM12	3
#define CS12	2
#define CS11	1
#define CS10	0
#define FOC1A	7
#define FOC1B	6

extern volatile uint8_t OCR0A;
#define OCR0A	OCR0A

extern volatile uint8_t OCR0B;
#define OCR0B	OCR0B

extern volatile uint8_t TCNT0;
#define TCNT0	TCNT0

extern volatile uint8_t TCCR0A;
#define TCCR0A	TCCR0A

extern volatile uint8_t TCCR0B;
#define TCCR0B	TCCR0B

extern volatile uint16_t OCR1A;
#define OCR1A	OCR1A

extern volatile uint16_t OCR1B;
#define OCR1B	OCR1B

extern volatile uint16_t ICR1;
#define ICR1	ICR1

extern volatile uint8_t TCNT1;
#define TCNT1	TCNT1

extern volatile uint8_t TCCR1A;
#define TCCR1A	TCCR1A

extern volatile uint8_t TCCR1B;
#define TCCR1B	TCCR1B

extern volatile uint8_t TCCR1C;
#define TCCR1C	TCCR1C


#define EEPM1	5
#define EEPM0	4
#define EERIE	3
#define EEMPE	2
#define EEPE	1
#define EERE	0
extern volatile uint8_t EECR;
#define EECR	EECR

extern volatile uint8_t EEDR;
#define EEDR	EEDR

extern volatile uint16_t EEAR;
#define EEAR	EEAR
#if __BYTE_ORDER == __LITTLE_ENDIAN
# define EEARL	(*(((uint8_t *)&EEAR) + 0))
# define EEARH	(*(((uint8_t *)&EEAR) + 1))
#else
# define ADCL	(*(((uint8_t *)&EEAR) + 1))
# define ADCH	(*(((uint8_t *)&EEAR) + 0))
#endif

#define E2END	0x3FF


extern volatile uint16_t ADC;
#define ADC	ADC
#define ADCW	ADC
#if __BYTE_ORDER == __LITTLE_ENDIAN
# define ADCL	(*(((uint8_t *)&ADC) + 0))
# define ADCH	(*(((uint8_t *)&ADC) + 1))
#else
# define ADCL	(*(((uint8_t *)&ADC) + 1))
# define ADCH	(*(((uint8_t *)&ADC) + 0))
#endif

#define ADEN	7
#define ADSC	6
#define ADATE	5
#define ADIF	4
#define ADIE	3
#define ADPS2	2
#define ADPS1	1
#define ADPS0	0
extern volatile uint8_t ADCSRA;
#define ADCSRA	ADCSRA

#define ACME	6
#define ADTS2	2
#define ADTS1	1
#define ADTS0	0
extern volatile uint8_t ADCSRB;
#define ADCSRB	ADCSRB

#define REFS1	7
#define REFS0	6
#define ADLAR	5
#define MUX3	3
#define MUX2	2
#define MUX1	1
#define MUX0	0
extern volatile uint8_t ADMUX;
#define ADMUX	ADMUX

#define ADC5D	5
#define ADC4D	4
#define ADC3D	3
#define ADC2D	2
#define ADC1D	1
#define ADC0D	0
extern volatile uint8_t DIDR0;
#define DIDR0	DIDR0

#define AIN1D	1
#define AIN0D	0
extern volatile uint8_t DIDR1;
#define DIDR1	DIDR1


#endif /* FAKE_IO_H_ */
