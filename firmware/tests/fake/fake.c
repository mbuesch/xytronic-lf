#include <avr/io.h>

volatile uint8_t MCUCSR;

volatile uint8_t PORTB;
volatile uint8_t PORTC;
volatile uint8_t PORTD;
volatile uint8_t DDRB;
volatile uint8_t DDRC;
volatile uint8_t DDRD;
volatile uint8_t PINB;
volatile uint8_t PINC;
volatile uint8_t PIND;

volatile uint8_t TIMSK0;
volatile uint8_t TIFR0;
volatile uint8_t TIMSK1;
volatile uint8_t TIFR1;

volatile uint8_t SREG;

volatile uint8_t OCR0A;
volatile uint8_t OCR0B;
volatile uint8_t TCNT0;
volatile uint8_t TCCR0A;
volatile uint8_t TCCR0B;
volatile uint16_t OCR1A;
volatile uint16_t OCR1B;
volatile uint16_t ICR1;
volatile uint8_t TCNT1;
volatile uint8_t TCCR1A;
volatile uint8_t TCCR1B;
volatile uint8_t TCCR1C;

volatile uint16_t ADC;
volatile uint8_t ADCSRA;
volatile uint8_t ADCSRB;
volatile uint8_t ADMUX;
volatile uint8_t DIDR0;
volatile uint8_t DIDR1;

volatile uint8_t EECR;
volatile uint8_t EEDR;
volatile uint16_t EEAR;
