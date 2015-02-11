#include "serialmouse.h"
#include <Arduino.h>
#include <timer2.h>

volatile unsigned char buf[3];
volatile char bitpos = 0;
volatile char bytepos = 0;

void sm_timer();
void sm_interrupt();
void sm_init();
void sm_last();

volatile long mousex, mousey;

void sm_last_bit()
{
	timer2_stop();

	if(bytepos > 2)
	{
		bytepos = 0;
		mousex +=   (char) (((buf[0] & 0x03) << 6) | (buf[1] & 0x7f));
		mousey += - (char) (((buf[0] & 0x0C) << 4) | (buf[2] & 0x7f));
	}

	attachInterrupt(PININT, sm_interrupt, CHANGE);
}

void sm_timer()
{
	char bit = digitalRead(PINMOUSE) ^ 1;
	
	buf[bytepos] = (buf[bytepos] >> 1) | (bit << 7);
	bitpos++;
	
	if(bitpos < 6)
	{
		timer2_init(152, 5, sm_timer);
		timer2_start();
	}
	else
	{
		buf[bytepos] = (buf[bytepos] >> 2);
		bitpos = 0;
		bytepos++;
		
		timer2_init(48, 5, sm_last_bit);
		timer2_start();
	}
}

void sm_interrupt()
{
	detachInterrupt(PININT);

	/* Wait 833.33 + 1/2 * 833.33 us */
	timer2_init(100, 5, sm_timer);
	timer2_start();
}

void sm_init()
{
	bitpos = 0;
	bytepos = 0;
	mousex = 0;
	mousey = 0;

	pinMode(PINMOUSE, INPUT);
	attachInterrupt(PININT, sm_interrupt, CHANGE);
}

