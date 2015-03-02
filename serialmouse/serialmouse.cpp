#include "serialmouse.h"
#include <Arduino.h>
#include <timer2.h>

unsigned char buf[3];
volatile char bitpos = 0;
volatile char bytepos = 0;
volatile char ps = 0;

static void (*mouse_func)(unsigned char *) = NULL;


void sm_timer();
void sm_interrupt();
void sm_init();
void sm_last();

volatile long mouse_rx, mouse_ry;
volatile char mouse_vx, mouse_vy;
volatile int mouse_ax, mouse_ay;

void debug()
{
	ps ^= 1;
	digitalWrite(13, ps);
}

void sm_last_bit()
{
	/*
	volatile char _mouse_vx, _mouse_vy;
	volatile int _mouse_ax, _mouse_ay;
	*/

	timer2_stop();

	if(bytepos > 2)
	{
		bytepos = 0;
		if(mouse_func) mouse_func(buf);
		/*
		_mouse_vx = (char) (((buf[0] & 0x03) << 6) | (buf[1] & 0x3f));
		_mouse_vy = (char) (((buf[0] & 0x0C) << 4) | (buf[2] & 0x3f));
		
		_mouse_ax = mouse_vx - _mouse_vx;
		_mouse_ay = mouse_vy - _mouse_vy;

		if(abs(_mouse_ax) < 15)
		{
			mouse_ax = _mouse_ax;
			mouse_ay = _mouse_ay;

			mouse_vx = _mouse_vx;
			mouse_vy = _mouse_vy;

			mouse_rx += _mouse_vx;
			mouse_ry += _mouse_vy;
		}
		else
		{
			digitalWrite(13, 1);
		}
		*/
	}



	attachInterrupt(PININT, sm_interrupt, RISING);
	
	/* Section 23.3.2: Remove previous interrups */
	//ACSR |= _BV(ACI);
	/* Section 23.3.2: Enable Analog Comparator interrupt */
	//ACSR |= _BV(ACIE);
}

char sm_check(char byte)
{

	if(byte == 0)
	{
		if((buf[0] & B11110000) != B11000000)
		{
			digitalWrite(13, 1);
			return 0;
		}
	}
	else
	{

		if((buf[byte] & B11000000) != B10000000)
		{
			digitalWrite(13, 1);
			return 0;
		}
	}
	digitalWrite(13, 0);
	return 1;
}


void sm_timer()
{
	char bit = digitalRead(PINMOUSE) ^ 1;
	
	buf[bytepos] = (buf[bytepos] >> 1) | (bit << 7);
	
	if(bitpos < 7)
	{
		bitpos++;
		timer2_init(152, 5, sm_timer);
		timer2_start();
	}
	else
	{
		//buf[bytepos] = (buf[bytepos] >> 1);
		bitpos = 0;
		if(sm_check(bytepos))
		{
			bytepos++;

			/* Wait 833.33*2 us */
			//timer2_init(48, 5, sm_last_bit);
			
			/* Wait 833.33 us */
			//timer2_init(152, 5, sm_last_bit);
			//timer2_start();

			/* Don't wait, start doing calculations */
			timer2_stop();
			sm_last_bit();
		}
		else
		{
			bytepos = 0;
			timer2_stop();
			/* Section 23.3.2: Remove previous interrups */
			//ACSR |= _BV(ACI);
			/* Section 23.3.2: Enable Analog Comparator interrupt */
			//ACSR |= _BV(ACIE);

			attachInterrupt(PININT, sm_interrupt, RISING);
		}
	}
}

void sm_interrupt()
{
	detachInterrupt(PININT);

	/* Wait 833.33 + 1/2 * 833.33 us */
	timer2_init(100, 5, sm_timer);
	timer2_start();
}

ISR(ANALOG_COMP_vect)
{
	/* Section 23.3.2: Disable Analog Comparator interrupt */
	//ACSR &= ~_BV(ACIE);

	//sm_interrupt();
}

void sm_init(void (*f)(unsigned char *))
{
	bitpos = 0;
	bytepos = 0;
	mouse_rx = 0;
	mouse_ry = 0;
	mouse_vx = 0;
	mouse_vy = 0;
	mouse_ax = 0;
	mouse_ay = 0;
	mouse_func = f;

	pinMode(13, OUTPUT);
	pinMode(PINMOUSE, INPUT);
	attachInterrupt(PININT, sm_interrupt, RISING);

	/* Section 23.1: Power on analog comparator */
	//PRADC |= _BV(ADC);

	/* TODO: MAGIC! */
	//ACSR = B01011011;

	/* Section 23.3.2: Remove previous interrups */
	//ACSR |= _BV(ACI);
	/* Section 23.3.2: Enable Analog Comparator interrupt */
	//ACSR |= _BV(ACIE);
	
	/* Section 23.3.2: Disable Analog Comparator interrupt */
	//ACSR &= ~_BV(ACIE);
}

