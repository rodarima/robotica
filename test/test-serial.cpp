#include <Arduino.h>
#include <timer2.h>

unsigned long t;
int index = 0;

#define N 9
int times[N] = {0, 4120, 8272, 16540, 22336, 0, 0, 0, 0};
long mtimes[N] = {0};
long cnt = 1;

void measure_bit()
{
	int i;
	for(i=0; i<10; i++)
	{
		if(digitalRead(2) == 0)
		{
			Serial.print("!");
		}
		delayMicroseconds(2);
	}
}

void handle_start_bit()
{
	unsigned long _t = micros();
	unsigned long d = _t - t;

	if(d >= 24820) 
	{
		index = 0;
		Serial.print("R");
	}

	if(index == 0)
	{
		t = _t;
	}
	else if(index > 0)
	{
		mtimes[index] += d;
		Serial.print(d);
		Serial.print("/");
		Serial.print((double)mtimes[index]/cnt);
		Serial.print("\t");

	}
	
	index = (index+1) % N;

	if(index == 0)
	{
		Serial.print("\n");
		cnt++;
	}
}

void sm_init()
{
	attachInterrupt(0, handle_start_bit, CHANGE);
}

char direction = 0;
long count = 0;

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
			/*
			digitalWrite(PINMR0, 0);
			digitalWrite(PINMR1, 1);
			digitalWrite(PINML0, 0);
			digitalWrite(PINML1, 1);
			*/
			analogWrite(PINMR0, 0);
			analogWrite(PINMR1, 100);
			//analogWrite(PINML0, 0);
			//analogWrite(PINML1, 100);
		}
		else
		{
			/*
			digitalWrite(PINMR0, 1);
			digitalWrite(PINMR1, 0);
			digitalWrite(PINML0, 1);
			digitalWrite(PINML1, 0);
			*/
			analogWrite(PINMR0, 100);
			analogWrite(PINMR1, 0);
			//analogWrite(PINML0, 100);
			//analogWrite(PINML1, 0);
		}
	}

	if(count > 100000)
	{
		count = -10000;
		//count = 0;
		direction ^= 1;
	}
	
	count++;
}

void setup()
{
	Serial.begin(115200);

	pinMode(2, INPUT);

	pinMode(PINMR0, OUTPUT);
	pinMode(PINMR1, OUTPUT);
	pinMode(PINML0, OUTPUT);
	pinMode(PINML1, OUTPUT);

	sm_init();
}

void loop()
{
	test_motors();
}
