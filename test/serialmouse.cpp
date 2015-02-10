#include <Arduino.h>
#include <timer2.h>

unsigned long t;
int index = 0;
int times[] = {0, 4120, 8272, 16540, 22336};

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
	cli();
	unsigned long _t = micros();
	unsigned long d = _t - t;


	if(d > 25000)
	{
		Serial.println("RESET");
		index = 0;
	}

	if(index != 0)
	{
		if(d > times[index] + 50 ||
		d < times[index] - 50)
		{
			Serial.print(index);
			Serial.print("E");
			Serial.println(d);
		}
		else
		{
			Serial.println(index);
		}
	}
	else
	{
		Serial.println(index);
		t = _t;
	}
	index = (index + 1) % 5;
	
	/*
	measure_bit();
	//Serial.print(" ");
	Serial.print(index);
	Serial.print(":");
	Serial.print(_t);
	Serial.print(" +");
	Serial.println(d/833.33);
	*/
	sei();
}

void sm_init()
{
	attachInterrupt(0, handle_start_bit, RISING);
}

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
		count = 0;
		direction ^= 1;
	}
	
	count++;
}

void setup()
{
	Serial.begin(115200);

	pinMode(2, INPUT);

	pinMode(PINMR0,OUTPUT);
	pinMode(PINMR1,OUTPUT);
	pinMode(PINML0,OUTPUT);
	pinMode(PINML1,OUTPUT);

	sm_init();
}

void loop()
{
	test_motors();
}
