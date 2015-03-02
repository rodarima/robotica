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
int val;

#define MAXY 10
#define VAL 120



void loop()
{
	//test_motors();
	if(x != mousex || y != mousey)
	{
		x = mousex;
		y = mousey;

		if(y > MAXY)
		{
			val = VAL-y < 0 ? 0 : VAL-y;
			if(direction)
			{
				digitalWrite(PINML0, val);
				digitalWrite(PINML1, 0);
			}
			else
			{
				digitalWrite(PINMR0, 0);
				digitalWrite(PINMR1, val);
			}
		}
		else if(y < -MAXY)
		{
			val = VAL+y < 0 ? 0 : VAL+y;
			if(direction)
			{
				digitalWrite(PINMR0, val);
				digitalWrite(PINMR1, 0);
			}
			else
			{
				digitalWrite(PINML0, 0);
				digitalWrite(PINML1, val);
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
				analogWrite(PINMR0, VAL);
				analogWrite(PINMR1, 0);
				analogWrite(PINML0, VAL);
				analogWrite(PINML1, 0);
			}
			else
			{
				analogWrite(PINMR0, 0);
				analogWrite(PINMR1, VAL);
				analogWrite(PINML0, 0);
				analogWrite(PINML1, VAL);
				/*
				digitalWrite(PINMR0, 0);
				digitalWrite(PINMR1, 1);
				digitalWrite(PINML0, 0);
				digitalWrite(PINML1, 1);
				*/
			}
		}
		
		if((x > 2000) && (direction == 0))
		{
			direction = 1;
			digitalWrite(PINMR0, 0);
			digitalWrite(PINMR1, 0);
			digitalWrite(PINML0, 0);
			digitalWrite(PINML1, 0);
			delay(500);
			/*
			digitalWrite(PINMR0, 1);
			digitalWrite(PINMR1, 0);
			digitalWrite(PINML0, 1);
			digitalWrite(PINML1, 0);
			*/	
			analogWrite(PINMR0, VAL);
			analogWrite(PINMR1, 0);
			analogWrite(PINML0, VAL);
			analogWrite(PINML1, 0);
		}
		else if((x < 0) && (direction == 1))
		{
			direction = 0;
			digitalWrite(PINMR0, 0);
			digitalWrite(PINMR1, 0);
			digitalWrite(PINML0, 0);
			digitalWrite(PINML1, 0);
			delay(500);
			/*
			digitalWrite(PINMR0, 0);
			digitalWrite(PINMR1, 1);
			digitalWrite(PINML0, 0);
			digitalWrite(PINML1, 1);
			*/
			analogWrite(PINMR0, 0);
			analogWrite(PINMR1, VAL);
			analogWrite(PINML0, 0);
			analogWrite(PINML1, VAL);
		}

		Serial.print("x=");
		Serial.print(x);
		Serial.print(" y=");
		Serial.println(y);
	}
}
