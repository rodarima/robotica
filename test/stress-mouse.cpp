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

#define VAL 255

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
			analogWrite(PINMR1, VAL);
			digitalWrite(PINML0, 0);
			analogWrite(PINML1, VAL);
		}
		else
		{
			analogWrite(PINMR0, VAL);
			digitalWrite(PINMR1, 0);
			analogWrite(PINML0, VAL);
			digitalWrite(PINML1, 0);
		}
	}

	if(count > 5000)
	{
		//count = -5000;
		count = 0;
		direction ^= 1;
	}
	
	count++;
}

void setup()
{
	Serial.begin(115200);
	sm_init();

	pinMode(PINMR0,OUTPUT);
	pinMode(PINMR1,OUTPUT);
	pinMode(PINML0,OUTPUT);
	pinMode(PINML1,OUTPUT);
			
			
	digitalWrite(PINMR0, 0);
	digitalWrite(PINMR1, 1);
	digitalWrite(PINML0, 0);
	digitalWrite(PINML1, 1);

}

long x = 0, y = 0;

#define MAX_X 1000
#define MAX_Y 9

void loop()
{
	test_motors();
/*
	if(x != mousex || y != mousey)
	{
		x = mousex;
		y = mousey;


		if(y > MAX_Y)
		{
			if(direction)
			{
				
				digitalWrite(PINMR0, 1);
				digitalWrite(PINMR1, 0);
				digitalWrite(PINML0, 0);
				digitalWrite(PINML1, 0);
			}
			else
			{
				digitalWrite(PINMR0, 0);
				digitalWrite(PINMR1, 0);
				digitalWrite(PINML0, 0);
				digitalWrite(PINML1, 1);
			}
		}
		else if(y < -MAX_Y)
		{
			if(direction)
			{
				
				digitalWrite(PINMR0, 0);
				digitalWrite(PINMR1, 0);
				digitalWrite(PINML0, 1);
				digitalWrite(PINML1, 0);
			}
			else
			{
				digitalWrite(PINMR0, 0);
				digitalWrite(PINMR1, 1);
				digitalWrite(PINML0, 0);
				digitalWrite(PINML1, 0);
			}
		}
		else
		{
			if(direction)
			{
			
			
				digitalWrite(PINMR0, 1);
				digitalWrite(PINMR1, 0);
				digitalWrite(PINML0, 1);
				digitalWrite(PINML1, 0);
				
			}
			else
			{
				digitalWrite(PINMR0, 0);
				digitalWrite(PINMR1, 1);
				digitalWrite(PINML0, 0);
				digitalWrite(PINML1, 1);
			}
		}
		
		if((x > MAX_X) && (direction != 1))
		{
			direction = 1;
			digitalWrite(PINMR0, 0);
			digitalWrite(PINMR1, 0);
			digitalWrite(PINML0, 0);
			digitalWrite(PINML1, 0);
			delay(500);
			digitalWrite(PINMR0, 1);
			digitalWrite(PINMR1, 0);
			digitalWrite(PINML0, 1);
			digitalWrite(PINML1, 0);
		}
		else if((x < 0) && (direction != 0))
		{
			direction = 0;
			digitalWrite(PINMR0, 0);
			digitalWrite(PINMR1, 0);
			digitalWrite(PINML0, 0);
			digitalWrite(PINML1, 0);
			delay(500);
			digitalWrite(PINMR0, 0);
			digitalWrite(PINMR1, 1);
			digitalWrite(PINML0, 0);
			digitalWrite(PINML1, 1);
		}

	}
*/
	if((count%10) == 0)
	{
		Serial.print("x=");
		Serial.print(mousex);
		Serial.print(" y=");
		Serial.println(mousey);
	}
}
