#include "motion.h"
#include <Arduino.h>
#include "odometry.h"
#include "pinout.h"

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

void motion_rotate(float theta)
{
	target_theta = theta;
}

void motion_rotate_delta(float theta_delta)
{
	target_theta+=theta_delta;
}

void motion_set_velocity(float v)
{
	velocity = v;
}

void motion_move(float d)
{
	distance = robot.rx + d;
}

#define VMIN	170
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
		*m0 = VOLT(p);
	}
}

void _motor_left_set(float p)
{
	_motor_set(&tml0, &tml1, p);
	analogWrite(PINML0, tml0);
	analogWrite(PINML1, tml1);
}
void _motor_right_set(float p)
{
	_motor_set(&tmr0, &tmr1, p);
	analogWrite(PINMR0, tmr0);
	analogWrite(PINMR1, tmr1);
}

void _motion_distance()
{
	if(robot.rx >= distance)
	{
		_motor_left_set(0);
		_motor_right_set(0);
	}
	else
	{
		_motor_left_set(1);
		_motor_right_set(1);
	}
}

void motion_update()
{
	_motion_distance();
}
