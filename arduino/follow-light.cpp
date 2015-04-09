#include <Arduino.h>
#include <serialmouse.h>
#include <odometry.h>
#include <motion.h>
#include <pinout.h>
#include <sonar.h>


#define GO_AHEAD 0
#define BACK 1
#define TURN 2
#define LIGHT 3
#define RANDOM 4

#define PMIN	0
#define PMAX	1

void print_light();
void motors_stop();
void light_update();
void set_behavior_random();

unsigned char _counter;
int snr0;
char stop = 0;

char behavior = GO_AHEAD;

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


#define OPENING	(50.0 / 360.0 * 2.0*PI)
#define ODELTA	(OPENING/2.0)
#define RIGHT	0
#define LEFT	1

float theta_min, theta_max, swing_dir = RIGHT;
float theta_light, light_max;


void set_behavior_go_ahead()
{
	behavior = GO_AHEAD;
	swing(robot.theta, ODELTA, PMIN, PMAX, NULL);
}

void update_behavior_go_ahead()
{
	Serial.print("Updating sonar\n");
	sonar_update(&snr0);
}

void _go_back_end_cb()
{
	//set_behavior_random();
	set_behavior_go_ahead();
}

void _go_back_stop_cb()
{
	Serial.print("Waiting to end back");
	motors(0, 0);
	motion_wait(1000, _go_back_end_cb);
}

void _go_back_wait_cb()
{
	Serial.print("Going back");
	motors(-1, -1);
	motion_wait(5000, _go_back_stop_cb);
}

void set_behavior_go_back()
{
	Serial.print("Waiting to go back");
	behavior = BACK;
	motors(0, 0);
	motion_wait(1000, _go_back_wait_cb);
}

void update_behavior_go_back()
{
	motion_wait_update();
}

void set_behavior_random()
{
	behavior = RANDOM;
	int angle = random(90);
	float rad = (float) angle / 360.0 * 2.0 * PI;
	rotate_cb(rad, PMAX, PMIN, motors_stop);
}

void update_behavior_random()
{
	rotation_update();
}

void motors_stop()
{
	Serial.println("MOTORS STOP");
	motors(0, 0);
}

void update_aperture()
{
	if(swing_dir == RIGHT)
	{
		if(robot.theta < theta_min)
		{
			print_light();
			swing(theta_light, ODELTA, PMIN, PMAX, update_aperture);
			print_light();
		}
	}
	else if(swing_dir == LEFT)
	{
		if(robot.theta > theta_max)
		{
			print_light();
			swing(theta_light, ODELTA, PMIN, PMAX, update_aperture);
			print_light();
		}
	}
//	theta_light = 0;
	light_max = 0;
}
void set_behavior_light()
{
	behavior = LIGHT;
	swing(robot.theta, ODELTA, PMIN, PMAX, update_aperture);
}

void update_behavior_light()
{
	light_update();
//	rotation_update();
	sonar_update(&snr0);
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
	Serial.print(", l:");
	Serial.print(theta_light);
	Serial.print("\n");
}
void print_light()
{
	Serial.print("th:");
	Serial.print(robot.theta);
	Serial.print(", t+:");
	Serial.print(theta_max);
	Serial.print(", t-:");
	Serial.print(theta_min);
	Serial.print(", ldr0+:");
	Serial.print(light_max);
	Serial.print(", theta+:");
	Serial.print(theta_light);
	Serial.print("\n");
}
void print_ldr()
{
	Serial.print("th:");
	Serial.print(robot.theta);
	Serial.print(", t+:");
	Serial.print(theta_max);
	Serial.print(", t-:");
	Serial.print(theta_min);
	Serial.print(", ldr0+:");
	Serial.print(light_max);
	Serial.print(", theta+:");
	Serial.print(theta_light);
	Serial.print("\n");
}

void light_update()
{
	float ldr1 = (float) analogRead(PIN_LDR1) / 1023.0;

	if(light_max < ldr1)
	{
		light_max = ldr1;
		theta_light = robot.theta;
	}

/*
	Serial.print("ldr1:");
	Serial.print(ldr1);
	Serial.print("\n");
*/
}

void select_behavior()
{

	if((snr0 > 0) && (snr0 < 800 ) && (behavior == GO_AHEAD))
	{
		set_behavior_go_back();
		return;
	}

	if((light_max > 0.2) && (behavior == GO_AHEAD))
	{
		set_behavior_light();
		return;
	}


	Serial.print("Behavior:");
	Serial.print((int)behavior);
	Serial.print("\n");

}

void update_behavior()
{
	switch(behavior)
	{
		case GO_AHEAD:	update_behavior_go_ahead();
		case BACK:	update_behavior_go_back();
		case RANDOM:	update_behavior_random();
		case LIGHT:	update_behavior_light();
	}

	select_behavior();
}

////////////////////////////////////////////////////////////////////////////////

void setup()
{
	Serial.begin(115200);
	Serial.print("Starting arduino\n");

	odometry_init();
	motion_init();
	sonar_init();

	set_behavior_go_ahead();
}

void loop()
{
	update_behavior();
	rotation_update();

	print_theta();
	//print_json();
	print_light();
	//

	//delay(100);

	while(stop);
}
