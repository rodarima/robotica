#include "motion.h"

#include <Arduino.h>
#include <odometry.h>
#include <pinout.h>

unsigned char mr0, mr1, ml0, ml1;
unsigned char tmr0, tmr1, tml0, tml1;
float target_theta;
float velocity;
float distance;
void (*motion_wait_cb)(void) = NULL;
void (*swing_cb)(void) = NULL;
void (*rotation_cb)(void) = NULL;

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

	rotation_cb = NULL;
	swing_cb = NULL;
	motion_wait_cb = NULL;
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

void motors(float l, float r)
{
	Serial.print("motors");
	Serial.print(l);
	Serial.print(r);
	Serial.print("\n");

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

#define R_CW	0	//Clockwise rotation
#define R_CCW	1	//Counter clockwise rotation

char rotation_dir;
float rotation_target;

void rotate_cb(float a, float pmin, float pmax, void (*cb)(void))
{
	rotation_target = a;
	rotation_cb = cb;

	Serial.print("rotate_cb angle:");
	Serial.print(a);
	Serial.print(" theta:");
	Serial.print(robot.theta);
	Serial.print(" pmax:");
	Serial.print(pmax);
	Serial.print(" pmin:");
	Serial.print(pmin);
	Serial.print("\n");

	/* Turn motors */
	if(robot.theta > a)
	{
		rotation_dir = R_CW;
		motors(pmax, pmin);
	}
	else
	{
		rotation_dir = R_CCW;
		motors(pmin, pmax);
	}

}

void print_rotation()
{
	Serial.print(rotation_dir == R_CW ? "CW" : "CCW");
	Serial.print(" th:");
	Serial.print(robot.theta);
	Serial.print(", target:");
	Serial.print(rotation_target);
	Serial.print("\n");
}

void rotation_update()
{
	//print_rotation();
	void (*cb)(void) = rotation_cb;
	if(!rotation_cb)
	{
		Serial.print("rotation cb = NULL\n");
		return;
	}

	if(rotation_dir == R_CW)
	{
		if(robot.theta < rotation_target)
		{
			rotation_cb = NULL;
			cb();
		}
	}
	else if(rotation_dir == R_CCW)
	{
		if(robot.theta > rotation_target)
		{
			rotation_cb = NULL;
			cb();
		}
	}

}

float swing_min, swing_max, swing_pmin, swing_pmax;
void _swing_min_cb();
void _swing_max_cb();

void print_swing()
{
	Serial.print("th:");
	Serial.print(robot.theta);
	Serial.print(", t+:");
	Serial.print(swing_max);
	Serial.print(", t-:");
	Serial.print(swing_min);
	Serial.print("\n");
}

void _swing_min_cb()
{
	Serial.print("swing min cb\n");
	if(swing_cb) swing_cb();
	print_swing();
	rotate_cb(swing_max, swing_pmin, swing_pmax, _swing_max_cb);
}
void _swing_max_cb()
{
	Serial.print("swing max cb\n");
	if(swing_cb) swing_cb();
	print_swing();
	rotate_cb(swing_min, swing_pmin, swing_pmax, _swing_min_cb);
}

void swing(float a, float delta, float pmin, float pmax, void (*cb)(void))
{
	swing_min = a - delta;
	swing_max = a + delta;
	swing_cb = cb;
	swing_pmin = pmin;
	swing_pmax = pmax;
	Serial.print("Swing\n");
	if(rotation_dir == R_CW)
		rotate_cb(swing_min, swing_pmin, swing_pmax, _swing_min_cb);
	else
		rotate_cb(swing_max, swing_pmin, swing_pmax, _swing_max_cb);
}

long motion_wait_timeout = 0;

void motion_wait(long ms, void (*cb)(void))
{
	motion_wait_timeout = millis() + ms;
	motion_wait_cb = cb;
}

void motion_wait_update()
{
	if(!motion_wait_cb) return;

	Serial.print("millis ");
	Serial.print(millis());
	Serial.print("timeout ");
	Serial.print(motion_wait_timeout);
	Serial.print("\n");

	if(millis() > motion_wait_timeout)
	{
		void (*cb)(void) = motion_wait_cb;
		motion_wait_cb = NULL;
		cb();
	}
}
