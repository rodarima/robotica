#include <Arduino.h>
#include <serialmouse.h>
#include <odometry.h>
#include <motion.h>
#include <pinout.h>
#include <sonar.h>

unsigned char _counter;
int snr0;
char stop = 0;

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


#define OPENING	(60.0 / 360.0 * 2.0*PI)
#define ODELTA	(OPENING/2.0)
#define RIGHT	0
#define LEFT	1

float theta_min, theta_max, swing_dir = RIGHT;

void swing()
{
	float theta;
	theta = robot.theta;
	theta_min = theta - ODELTA;
	theta_max = theta + ODELTA;
	motion_rotate(theta_min);
}

void update_swing()
{
	float theta = robot.theta;
	if(swing_dir == RIGHT)
	{
		if(theta < theta_min)
		{
			motion_rotate(theta_max);
			swing_dir = LEFT;
		}
	}
	else if(swing_dir == LEFT)
	{
		if(theta > theta_max)
		{
			motion_rotate(theta_min);
			swing_dir = RIGHT;
		}
	}
}

void print_theta()
{
	Serial.print("th:");
	Serial.print(robot.theta);
	Serial.print(", t+:");
	Serial.print(theta_max);
	Serial.print(", t-:");
	Serial.print(theta_min);
	Serial.print(", snr0:");
	Serial.print(snr0);
	Serial.print("\n");
}

void obstacles_update()
{
	sonar_update(&snr0);
	if(snr0 == -1) return;

	if(snr0 < 800)
	{
		stop = 1;
		motors(0, 0);
	}
}

////////////////////////////////////////////////////////////////////////////////

void setup()
{
	Serial.begin(115200);

	odometry_init();
	motion_init();
	sonar_init();
	swing();
}

void loop()
{
	update_swing();
	rotation_update();
	obstacles_update();
	print_theta();
	print_json();

	while(stop);
}
