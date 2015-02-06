#include <Arduino.h>

void setup()
{
	pinMode(13, OUTPUT);

	noInterrupts();	// disable all interrupts
	TCCR1A = 0;
	TCCR1B = 0;
	TCNT1  = 0;

	OCR1A = 20000;
	TCCR1B |= (1 << WGM12);
	TCCR1B |= (1 << CS12);	//Preescaler = 1
	TIMSK1 |= (1 << OCIE1A);
	interrupts();
}

ISR(TIMER1_COMPA_vect)
{
	digitalWrite(13, digitalRead(13) ^ 1);
}

void loop()
{
	int i = 0;
}
