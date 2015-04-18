#include "serialmouse.h"

#include <Arduino.h>
#include <timer2.h>
#include <pinout.h>

unsigned char buf[3];
volatile char bitpos = 0;
volatile char bytepos = 0;

static void (*mouse_func)(unsigned char *) = NULL;


void sm_timer();
void sm_interrupt();
void sm_init();
void sm_last();

void debug(char v)
{
	digitalWrite(13, v);
}

void sm_last_bit()
{
	timer2_stop();

	if(bytepos > 2)
	{
		bytepos = 0;
		if(mouse_func) mouse_func(buf);
	}


	attachInterrupt(PININT, sm_interrupt, RISING);
}

char sm_check(char byte)
{

	if(byte == 0)
	{
		if((buf[0] & B11110000) != B11000000)
		{
			debug(1);
			return 0;
		}
	}
	else
	{

		if((buf[byte] & B11000000) != B10000000)
		{
			debug(1);
			return 0;
		}
	}
	debug(0);

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

void sm_init(void (*f)(unsigned char *))
{
	bitpos = 0;
	bytepos = 0;
	mouse_func = f;

	pinMode(13, OUTPUT);
	pinMode(PINMOUSE, INPUT);
	attachInterrupt(PININT, sm_interrupt, RISING);
}

