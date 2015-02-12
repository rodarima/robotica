#include <Arduino.h>
#include <serialmouse.h>

char direction = 0;
int count = 0;

#define PINMR0	9
#define PINMR1	10
#define PINML0	5
#define PINML1	6

#define PINSE	11
#define PINSR	12

void test_motors()
{
	if(count < 0)
	{
		digitalWrite(PINMR0, 0);
		digitalWrite(PINMR1, 0);
		digitalWrite(PINML0, 0);
		digitalWrite(PINML1, 0);
	}
	else
	{
		if(direction)
		{
			digitalWrite(PINMR0, 0);
			digitalWrite(PINMR1, 1);
			digitalWrite(PINML0, 0);
			digitalWrite(PINML1, 1);
		}
		else
		{
			digitalWrite(PINMR0, 1);
			digitalWrite(PINMR1, 0);
			digitalWrite(PINML0, 1);
			digitalWrite(PINML1, 0);
		}
	}

	if(count > 10000)
	{
		count = -5000;
		//count = 0;
		direction ^= 1;
	}
	
	count++;
}

void setup()
{
	Serial.begin(115200);

	pinMode(PINMR0,OUTPUT);
	pinMode(PINMR1,OUTPUT);
	pinMode(PINML0,OUTPUT);
	pinMode(PINML1,OUTPUT);
			
	/*
	digitalWrite(PINMR0, 0);
	digitalWrite(PINMR1, 1);
	digitalWrite(PINML0, 0);
	digitalWrite(PINML1, 1);
	*/

	sm_init();
}

long x = 0, y = 0;

void loop()
{
	//test_motors();
	if(x != mousex || y != mousey)
	{
		x = mousex;
		y = mousey;

		if(y > 10)
		{
			if(direction)
			{
				/*
				digitalWrite(PINMR0, 0);
				digitalWrite(PINMR1, 0);
				digitalWrite(PINML0, 0);
				digitalWrite(PINML1, 1);*/
			}
			else
			{
				digitalWrite(PINMR0, 0);
				digitalWrite(PINMR1, 0);
			}
		}
		else if(y < -10)
		{
			if(direction)
			{
				/*
				digitalWrite(PINMR0, 0);
				digitalWrite(PINMR1, 1);
				digitalWrite(PINML0, 0);
				digitalWrite(PINML1, 0);*/
			}
			else
			{
				digitalWrite(PINML0, 0);
				digitalWrite(PINML1, 0);
			}
		}
		else
		{
			if(direction)
			{
				/*
				digitalWrite(PINMR0, 1);
				digitalWrite(PINMR1, 0);
				digitalWrite(PINML0, 1);
				digitalWrite(PINML1, 0);
				*/
			}
			else
			{
				digitalWrite(PINMR0, 0);
				digitalWrite(PINMR1, 1);
				digitalWrite(PINML0, 0);
				digitalWrite(PINML1, 1);
			}
		}
		
		if(x > 2000)
		{
			direction = 1;
			digitalWrite(PINMR0, 0);
			digitalWrite(PINMR1, 0);
			digitalWrite(PINML0, 0);
			digitalWrite(PINML1, 0);
		}

		Serial.print("x=");
		Serial.print(x);
		Serial.print(" y=");
		Serial.println(y);
	}
}
