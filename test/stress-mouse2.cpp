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
			digitalWrite(PINMR1, 0);
			analogWrite(PINMR0, VAL);
			digitalWrite(PINML0, 0);
			analogWrite(PINML1, VAL);
		}
		else
		{
			analogWrite(PINMR1, VAL);
			digitalWrite(PINMR0, 0);
			analogWrite(PINML0, VAL);
			digitalWrite(PINML1, 0);
		}
	}

	if(count > 20000)
	{
		//count = 0;
		count = -5000;
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

long rx = 0, ry = 0;
long time = 0;

#define MAX_X 1000
#define MAX_Y 9

void loop()
{
	test_motors();
	if(time == 0) time = millis();
	if(time < millis())
	{
		time = millis() + 50;

		Serial.print("rx=");
		Serial.print(mouse_rx);
		Serial.print("\try=");
		Serial.print(mouse_ry);
		Serial.print("\tvx=");
		Serial.print(mouse_vx, DEC);
		Serial.print("\tvy=");
		Serial.print(mouse_vy, DEC);
		Serial.print("\tax=");
		Serial.print(mouse_ax);
		Serial.print("\tay=");
		Serial.println(mouse_ay);
	}
}
