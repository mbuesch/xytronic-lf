#define PROGMEM

#define OCIE0B	2
#define OCIE0A	1
#define TOIE0	0
extern volatile unsigned char TIMSK0;
#define TIMSK0	TIMSK0

#define OCF0B	2
#define OCF0A	1
#define TOV0	0
extern volatile unsigned char TIFR0;
#define TIFR0	TIFR0

#define ICIE1	5
#define OCIE1B	2
#define OCIE1A	1
#define TOIE1	0
extern volatile unsigned char TIMSK1;
#define TIMSK1	TIMSK1

#define ICF1	5
#define OCF1B	2
#define OCF1A	1
#define TOV1	0
extern volatile unsigned char TIFR1;
#define TIFR1	TIFR1

#define SREG_C	0
#define SREG_Z	1
#define SREG_N	2
#define SREG_V	3
#define SREG_S	4
#define SREG_H	5
#define SREG_T	6
#define SREG_I	7
extern volatile unsigned char SREG;
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

extern volatile unsigned char OCR0A;
#define OCR0A	OCR0A

extern volatile unsigned char OCR0B;
#define OCR0B	OCR0B

extern volatile unsigned char TCNT0;
#define TCNT0	TCNT0

extern volatile unsigned char TCCR0A;
#define TCCR0A	TCCR0A

extern volatile unsigned char TCCR0B;
#define TCCR0B	TCCR0B

extern volatile unsigned char OCR1A;
#define OCR1A	OCR1A

extern volatile unsigned char OCR1B;
#define OCR1B	OCR1B

extern volatile unsigned char ICR1;
#define ICR1	ICR1

extern volatile unsigned char TCNT1;
#define TCNT1	TCNT1

extern volatile unsigned char TCCR1A;
#define TCCR1A	TCCR1A

extern volatile unsigned char TCCR1B;
#define TCCR1B	TCCR1B

extern volatile unsigned char TCCR1C;
#define TCCR1C	TCCR1C
