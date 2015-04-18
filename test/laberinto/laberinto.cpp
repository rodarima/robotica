#include <Arduino.h>
#include <serialmouse.h>
#include "odometry/odometry.h"
#include "motion/motion.h"
#include "ldr/ldr.h"
#include "pinout.h"

unsigned char _counter;

float T = 0.0;

void follow_update()
{
	T = millis() * 1e-3 / 10;
	float Vr = (1.0 + pow(sin(T), 3))/2.0;
	float Vl = (1.0 + pow(sin(T+PI), 3))/2.0;
	motor_left(Vl);
	motor_right(Vr);
	/*
	Serial.print("Vr:");
	Serial.print(Vr);
	Serial.print(" Vl:");
	Serial.println(Vl);
	*/
}

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
	Serial.print("\"x\":");
	Serial.print(robot.x);
	Serial.print(", \"y\":");
	Serial.print(robot.y);
	Serial.print(", \"rx\":");
	Serial.print(robot.rx);
	Serial.print(", \"ry\":");
	Serial.print(robot.ry);
	Serial.print(", \"theta\":");
	Serial.print(robot.theta);
	Serial.print(", \"time\":");
	Serial.print(micros());
	Serial.print("}");
	Serial.println("}");
}
void print_csv()
{
	Serial.print(robot.rx);
	Serial.print(",");
	Serial.print(micros());
	Serial.print("\n");
}

void loop()
{
	follow_update();
	//motion_update();
	ldr_update();

	if(_counter != robot.counter)
	{
		_counter = robot.counter;
		print_json();
	}
	delay(20);
}
