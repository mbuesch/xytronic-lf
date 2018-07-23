#include <avr/io.h>

FakeIO<uint8_t> MCUCSR;

FakeIO<uint8_t> PORTB;
FakeIO<uint8_t> PORTC;
FakeIO<uint8_t> PORTD;
FakeIO<uint8_t> DDRB;
FakeIO<uint8_t> DDRC;
FakeIO<uint8_t> DDRD;
FakeIO<uint8_t> PINB;
FakeIO<uint8_t> PINC;
FakeIO<uint8_t> PIND;

FakeIO<uint8_t> TIMSK0;
FakeIO<uint8_t> TIFR0;
FakeIO<uint8_t> TIMSK1;
FakeIO<uint8_t> TIFR1;

FakeIO<uint8_t> SREG;

FakeIO<uint8_t> OCR0A;
FakeIO<uint8_t> OCR0B;
FakeIO<uint8_t> TCNT0;
FakeIO<uint8_t> TCCR0A;
FakeIO<uint8_t> TCCR0B;
FakeIO<uint16_t> OCR1A;
FakeIO<uint16_t> OCR1B;
FakeIO<uint16_t> ICR1;
FakeIO<uint8_t> TCNT1;
FakeIO<uint8_t> TCCR1A;
FakeIO<uint8_t> TCCR1B;
FakeIO<uint8_t> TCCR1C;

FakeIO<uint16_t> ADC;
FakeIO<uint8_t> ADCSRA;
FakeIO<uint8_t> ADCSRB;
FakeIO<uint8_t> ADMUX;
FakeIO<uint8_t> DIDR0;
FakeIO<uint8_t> DIDR1;

FakeIO<uint8_t> EECR;
FakeIO<uint8_t> EEDR;
FakeIO<ee_addr_t> EEAR;

FakeIO<uint8_t> UCSR0A;
FakeIO<uint8_t> UCSR0B;
FakeIO<uint8_t> UCSR0C;
FakeIO<uint16_t> UBRR0;
FakeIO<uint8_t> UDR0;

void fakeio_reset_all(void)
{
	MCUCSR.reset();

	PORTB.reset();
	PORTC.reset();
	PORTD.reset();
	DDRB.reset();
	DDRC.reset();
	DDRD.reset();
	PINB.reset();
	PINC.reset();
	PIND.reset();

	TIMSK0.reset();
	TIFR0.reset();
	TIMSK1.reset();
	TIFR1.reset();

	SREG.reset();

	OCR0A.reset();
	OCR0B.reset();
	TCNT0.reset();
	TCCR0A.reset();
	TCCR0B.reset();
	OCR1A.reset();
	OCR1B.reset();
	ICR1.reset();
	TCNT1.reset();
	TCCR1A.reset();
	TCCR1B.reset();
	TCCR1C.reset();

	ADC.reset();
	ADCSRA.reset();
	ADCSRB.reset();
	ADMUX.reset();
	DIDR0.reset();
	DIDR1.reset();

	EECR.reset();
	EEDR.reset();
	EEAR.reset();

	UCSR0A.reset();
	UCSR0B.reset();
	UCSR0C.reset();
	UBRR0.reset();
	UDR0.reset();
}
