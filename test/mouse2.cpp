#include <Arduino.h>
//#include <TimerOne.h>

int s = 0;
unsigned long t[100];
int ti = 0;

void start_bit()
{
	s++;
	//detachInterrupt(0);
}

void up()
{
	if(ti >= 100) ti = 99;

	if(ti == 0) t[ti++] = micros();
	else t[ti++] = micros() - t[0];

	if(ti >= 5)
	{
		s++;
		//ti = 0;
	}
}

void setup()  
{
	Serial.begin(115200);
	attachInterrupt(0, start_bit, RISING);
	//attachInterrupt(0, up, RISING);
}

unsigned char buf[100];
int i, j = 0, k = 0, l;
unsigned long d[4] = {0};

#define DELAY1	214
#define DELAY2	800

void loop()
{

	if(s > 0)
	{
		if(s != 1)
		{
			delay(30);
			s = 0;
			return;
		}
		
		if(k==0)
			d[k] = micros();
		else
			d[k] = micros() - d[0];
		
		delayMicroseconds(DELAY1);

		for(i = 0; i < 7; i++)
		{
			delayMicroseconds(DELAY2);
			buf[j++] = digitalRead(2);
		}
		delayMicroseconds(2*DELAY2);
		
		/*
		if(k == 2)
		{
			for(i = 0; i < 7; i++)
			{
				delayMicroseconds(833);
				buf[j++] = digitalRead(2);
			}
		}*/


		if(k == 2)
		{
			for(i = 0; i < 3; i++)
			{
				for(l = 0; l < 7; l++)
				{
					Serial.print(1-buf[i*7+l]);
				}
				Serial.print(" ");
				Serial.print(d[i]);
				Serial.print("\t");
			}
			Serial.print("\n");
			k = 0;
			j = 0;
			//delay(60);
		}
		else
		{
			k++;
		}
		
		s = 0;
		//attachInterrupt(0, start_bit, RISING);
	}
}
