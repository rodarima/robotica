#include "motion.h"

#include <Arduino.h>
#include <odometry.h>
#include <pinout.h>

unsigned char mr0, mr1, ml0, ml1;
unsigned char tmr0, tmr1, tml0, tml1;
float target_theta;
float velocity;
float distance;

void motion_init();

void motion_rotate(float theta);
void motion_rotate_delta(float theta_delta);

void motion_update();


void motion_init()
{
	mr0 = 0;
	mr1 = 0;
	ml0 = 0;
	ml1 = 0;

	tmr0 = 0;
	tmr1 = 0;
	tml0 = 0;
	tml1 = 0;

	target_theta = 0;
}

void motion_set_velocity(float v)
{
	velocity = v;
}

void motion_move(float d)
{
	distance = robot.rx + d;
}

#define VMIN	0
#define VMAX	255
#define VRAN	(VMAX - VMIN)
#define VOLT(p)	((p) * VRAN + VMIN)

void _motor_set(unsigned char *m0, unsigned char *m1, float p)
{
	if(p == 0.0)
	{
		*m0 = 0;
		*m1 = 0;
	}
	else if(p > 0)
	{
		*m0 = 0;
		*m1 = VOLT(p);
	}
	else
	{
		*m1 = 0;
		*m0 = VOLT(-p);
	}
}

void motor_left(float p)
{
	_motor_set(&tml0, &tml1, p);
	analogWrite(PIN_MTL0, tml0);
	analogWrite(PIN_MTL1, tml1);
}
void motor_right(float p)
{
	_motor_set(&tmr0, &tmr1, p);
	analogWrite(PIN_MTR0, tmr0);
	analogWrite(PIN_MTR1, tmr1);
}

void motors(float r, float l)
{
	motor_right(r);
	motor_left(l);
}

void _motion_distance()
{
	if(robot.rx >= distance)
	{
		motor_left(0);
		motor_right(0);
	}
	else
	{
		motor_left(1);
		motor_right(1);
	}
}

void motion_update()
{
	_motion_distance();
}

void motion_rotate(float a)
{
	target_theta = a;
}

//#define THETA_MAX	5

//float last_theta[THETA_MAX];

void correct_theta(float k)
{
	if(k > 0.0)
	{
		motors(k, 0);
	}
	else if(k < 0.0)
	{
		motors(0, -k);
	}
}

void rotation_update()
{
	if(robot.theta < target_theta)
	{
		motors(1, 0);
	}
	else if(robot.theta > target_theta)
	{
		motors(0, 1);
	}
}
