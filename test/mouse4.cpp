#include <Arduino.h>

static volatile unsigned long rx_time;
static volatile uint16_t cancer;
static volatile uint8_t go0 = 0;

void store_byte(uint8_t byte);
int read_byte(uint8_t *buf);
void handle_bit();
void handle_interrupt();

void err()
{
	digitalWrite(13, HIGH);
}

void handle_bit()
{
	//cli();
	
	unsigned long diff_time = micros() - rx_time;

	
	//Serial.print("TIME:");
	//Serial.print(F_CPU);
	//Serial.print(" ");
	//Serial.print(diff_time);
	//Serial.print(" ");
	//Serial.print(TCNT2);
	//Serial.print(" ");
	//Serial.print(TIFR2, HEX);
	//Serial.print(" ");
	//Serial.print(cancer);

	//Serial.print(" TCNT1:");
	//Serial.print((unsigned long)TCNT1);
	//Serial.print(" TIFR1:");
	//Serial.print(TIFR1, HEX);
	//Serial.print("\n");
	if((diff_time > 1249+20) ||
		(diff_time < 1249-20))
		err();
	
	go0 = 0;
	attachInterrupt(0, handle_interrupt, CHANGE);
	//sei();
}

void handle_interrupt()
{
	
	//EIMSK &= ~(1<<INT0);

	/*
	if((rx_mode == RISING) && (rx_actual_bit == 7))
	{
		uint16_t diff_time = micros() - rx_time;
		if((diff_time < 7000) ||
			(diff_time > 6600)) err();
	}
	*/
	
	cli();

	detachInterrupt(0);
	
	rx_time = micros();
	//cancer = 60001;

	/*
	TCCR1B = 0; //Stop timer1
	TIMSK1 = 0; //Clear all timer1 interrupts
	//TIFR1  = 0; //Clear all timer1 flags interrupts
	TIFR1 = 0xff; //Disable all passed interrupts

	//OCR1A = (uint8_t) 13333.333*1.5 ;
	OCR1A = 20000-1;

	TCCR1A = 0; //Clear timer1 control register
	TCNT1  = 1; //Set timer1 value to 0
	
	//Clear Timer on Compare Match Mode:
	//TCCR1B |= (1 << WGM12);

	//Enable timer compare interrupt
	TIMSK1 = (1 << OCIE1A);

	//Set prescaler to 1
	TCCR1B |= (1 << CS10);
	*/

	//    Set timer2
	/*
	
	//Stop timer2
	TCCR2B = 0;
	
	//Set timer2 value to 0
	TCNT2  = 0;

	//Set Timer2 Compare Register A
	OCR2A = 156;

	// Set Timer2 mask to compare register A
	TIMSK2 = _BV(OCIE1A);

	//Set prescaler to 128
	TCCR2B = _BV(CS22) | _BV(CS20);
	*/

	//    Set timer2 in overflow mode
	
	//Disable timer2 overflow interrupt
	//TIMSK2 &= ~_BV(TOIE2);
	//TIMSK2 = 0;
	
	// Set normal mode using WGM*
	//TCCR2B &= ~_BV(WGM22);
	//TCCR2A &= ~(_BV(WGM21) | _BV(WGM20));
	TCCR2B = 0;
	TCCR2A = 0;
	TIMSK2 = 0;
	
	//Set source clock I/O clk 17.11.8
	//ASSR &= ~_BV(AS2);
	
	// Set Timer2 mask to overflow
	//TIMSK2 &= ~_BV(OCIE2A);

	TCCR2B = _BV(CS22) | _BV(CS20);

	//Set timer2 value
	TCNT2 = 256-156;
	TIMSK2 = (1<<TOIE2);

	//Set prescaler to 128
	//TCCR2B |= _BV(CS22) | _BV(CS20);
	//TCCR2B &= ~_BV(CS21);
	TCCR2B = _BV(CS22) | _BV(CS20);

	//Clear previous interrupts
	//TIFR2 |= _BV(OCF2B) | _BV(OCF2A) | _BV(TOV2);
	TIFR2 = _BV(OCF2B) | _BV(OCF2A) | _BV(TOV2);
	//TIFR2 = 0xff;




	sei();

}

volatile uint8_t att = 0;

void timer2()
{

	TCCR2B = 0; //Stop timer2
	TIMSK2 = 0;

	ASSR &= ~(1<<AS2);
	
	//Set timer2 value
	TCNT2 = 180;

	//Set prescaler to 128
	TCCR2B = _BV(CS22);

	//Clear previous interrupts
	TIFR2 = _BV(OCF2B) | _BV(OCF2A) | _BV(TOV2);

	// Set Timer2 mask to overflow
	TIMSK2 = _BV(TOIE2);
}

ISR(TIMER2_OVF_vect)
{
	TCCR2B = 0; //Stop timer1
	TIMSK2 &= ~(1<<TOIE2);
	/*
	if(!att)
	{
		att++;
		attachInterrupt(0, handle_interrupt, CHANGE);
	}
	else
	{
		handle_bit();
	}
	*/
	handle_bit();
}

void setup()  
{
	micros();
	pinMode(13, OUTPUT);
	//pinMode(2, INPUT);

	digitalWrite(13, LOW);
	Serial.begin(115200);
	//timer2();
	attachInterrupt(0, handle_interrupt, CHANGE);
}

void loop()
{
	int i;
	while(1) i++;
}
