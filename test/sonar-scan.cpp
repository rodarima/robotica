#include <Arduino.h>
#include <serialmouse.h>

#define PINML0	9
#define PINML1	10
#define PINMR0	5
#define PINMR1	6

#define PINSNRT 11
#define PINSNRE 12

#define SPEED	150

#define TIME_RUN	5000
#define TIME_STOP	500

#define Y_MAX	500

void setup()
{
	Serial.begin(115200);
	pinMode(PINSNRT, OUTPUT);
	pinMode(PINSNRE, INPUT);

	pinMode(PINMR0,OUTPUT);
	pinMode(PINMR1,OUTPUT);
	pinMode(PINML0,OUTPUT);
	pinMode(PINML1,OUTPUT);

	sm_init();

}

int sonar()
{
	digitalWrite(PINSNRT, 0);
	delayMicroseconds(5);
	digitalWrite(PINSNRT, 1);
	delayMicroseconds(10);

	return pulseIn(PINSNRE, HIGH, 10000);
}

void set_motor(int d)
{
	if(d == 5)
	{
		analogWrite(PINML0, 0);
		analogWrite(PINML1, 0);
		analogWrite(PINMR0, 0);
		analogWrite(PINMR1, 0);
	}
	else if(d == 6)
	{
		analogWrite(PINML0, 0);
		analogWrite(PINML1, SPEED);
		analogWrite(PINMR0, SPEED);
		analogWrite(PINMR1, 0);
	}
	else if(d == 4)
	{
		analogWrite(PINML0, SPEED);
		analogWrite(PINML1, 0);
		analogWrite(PINMR0, 0);
		analogWrite(PINMR1, SPEED);
	}
}

int change_dir(int d)
{
	if(d == 6) return 4;
	return 6;
}

int state = 0, dir = 6, snr0, x, y;
unsigned long time_stop = 0, now;

void print_json()
{
	Serial.print("{\"x\":");
	Serial.print(x);
	Serial.print(", \"y\":");
	Serial.print(y);
	Serial.print(", \"snr0\":");
	Serial.print(snr0);
	Serial.print("}\n");
}

void loop()
{
	now = millis();
	x = mousex;
	y = mousey;
	
	if(state == 0)
	{
		if(time_stop < now)
		{
			state = 1;
			set_motor(dir);
			dir = change_dir(dir);
		}
	}
	else
	{
		if(dir == 4)
		{
			if(mousey > Y_MAX)
			{
				state = 0;
				set_motor(5);
				time_stop = now + TIME_STOP;
			}
		}
		else if(dir == 6)
		{
			if(mousey < -Y_MAX)
			{
				state = 0;
				set_motor(5);
				time_stop = now + TIME_STOP;
			}
		}
	}

	snr0 = sonar();

	print_json();

	delay(20);

}

