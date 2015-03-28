#include <Arduino.h>
#include <serialmouse.h>
#include <odometry.h>
#include <motion.h>
#include <pinout.h>

unsigned char _counter;

void setup()
{
	Serial.begin(115200);

	odometry_init();
	motion_init();
}

void print_json()
{
	Serial.print("{");

	Serial.print("\"pos\":{");
	Serial.print("\"sx\":");
	Serial.print(robot.sx);
	Serial.print(", \"sy\":");
	Serial.print(robot.sy);
	Serial.print(", \"rx\":");
	Serial.print(robot.rx);
	Serial.print(", \"ry\":");
	Serial.print(robot.ry);
	Serial.print(", \"theta\":");
	Serial.print(robot.theta/(2*PI)*360);
	Serial.print(", \"r\":");
	Serial.print(robot.r);
	Serial.print("}");
	Serial.println("}");
}
void print_csv()
{
	Serial.print(robot.r);
	Serial.print(", ");
	Serial.print(micros());
	Serial.print("\n");
}

#define RUN	2000000UL
#define SLEEP	1000000UL
#define SKIP	 500000UL
unsigned long t1, t2 = 0, t3, t4;
float p = 0.2;
char run = 1;
char di = 0;

void loop()
{
	t1 = micros();

	if(t1 >= t2)
	{
		if(run == 0)
		{
			p += 0.05;
			Serial.print(p);
			Serial.print("\t");
			t2 = micros() + RUN;
			t3 = micros();
			motor_left(p);
			motor_right(p);
			odometry_init();
			run = 0;
			di = 1;
		}
		else if(run == 1)
		{
			t2 = micros() + SLEEP;
			//motor_left(0);
			//motor_right(0);
			run = 0;
		}
		else if(run == 2)	//Volver al comienzo
		{
			motor_left(-1);
			motor_right(-1);
			while(robot.sy > 0);
			motor_left(0);
			motor_right(0);
			t2 = micros() + SLEEP;
			run = 0;
		}
	}

	/* Problemas de punto flotante */
	if((p > 1.01) && (run == 0))
	{
		motor_left(0);
		motor_right(0);
		while (1);
	}

	if(di>0)
	{
		if((t1 > t3 + SKIP) and (run == 0) and (di == 1))
		{
			robot.r = 0;
			di = 2;
			t4 = micros();
		}
		else if((t1 > t2 - SKIP) and (run == 0) and (di == 2))
		{
			Serial.print(robot.r);
			Serial.print("\t");
			Serial.print(t1 - t4);
			Serial.print("\n");
			di = 0;
		}
	}
/*
	if(_counter != robot.counter)
	{
		_counter = robot.counter;
		Serial.print(robot.r);
		Serial.print(", ");
		Serial.print(t1 - t3);
		Serial.print("\n");
	}
*/
/*
	if(_counter != robot.counter)
	{
		_counter = robot.counter;
		print_csv();
	}
*/
	//delay(20);
}
