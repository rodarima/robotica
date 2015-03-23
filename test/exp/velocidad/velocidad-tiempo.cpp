#include <Arduino.h>
#include <serialmouse.h>
#include "odometry.h"
#include "motion.h"
#include "pinout.h"

unsigned char _counter;

void setup()
{
	Serial.begin(115200);

	odometry_init();
	motion_init();
	motion_move(200);
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
	motion_update();

	if(_counter != robot.counter)
	{
		_counter = robot.counter;
		print_csv();
	}
	//delay(20);
}
