#include "motion.h"

#include <Arduino.h>
#include <odometry.h>
#include <pinout.h>

void (*wait_cb)(void) = NULL;
void (*rotation_cb)(void) = NULL;
void (*straight_cb)(void) = NULL;
void (*stop_cb)(void) = NULL;

void (*swing_cb)(void) = NULL;

void print_rotation();

void motion_rotate(float theta);
void motion_rotate_delta(float theta_delta);

void motion_update();

void callback(void (**cb)(void));

#define VMIN	0
#define VMAX	255
#define VRAN	(VMAX - VMIN)
#define VOLT(p)	((p) * VRAN + VMIN)

void motor_set(unsigned char *m0, unsigned char *m1, float p)
{
	if(p == 0.0)
	{
		*m0 = 0;
		*m1 = 0;
	}
	else if(p > 0)
	{
		if(p > 1) p = 1;
		*m0 = 0;
		*m1 = VOLT(p);
	}
	else
	{
		if(p < -1) p = -1;
		*m1 = 0;
		*m0 = VOLT(-p);
	}
}

void motor_left(float p)
{
	unsigned char tml0, tml1;
//	Serial.print("\tml:");
//	Serial.print(p);
	motor_set(&tml0, &tml1, p);
	analogWrite(PIN_MTL0, tml0);
	analogWrite(PIN_MTL1, tml1);
}
void motor_right(float p)
{
	unsigned char tmr0, tmr1;
//	Serial.print("\tmr:");
//	Serial.print(p);
	motor_set(&tmr0, &tmr1, p);
	analogWrite(PIN_MTR0, tmr0);
	analogWrite(PIN_MTR1, tmr1);
}

void motors(float l, float r)
{
/*
	Serial.print("\tm[");
	Serial.print(l);
	Serial.print(", ");
	Serial.print(r);
	Serial.print("]");
*/
	if(robot.ml == l && robot.mr == r) return;

	robot.ml = l;
	robot.mr = r;
	if(l == 0 && r == 0)
	{
		robot.vmax = 0;
	}
	motor_right(r);
	motor_left(l);
}

#define R_CW	0	//Clockwise rotation
#define R_CCW	1	//Counter clockwise rotation

char rotation_dir;
float rotation_target;

void motion_rotate(float a, float pmin, float pmax, void (*cb)(void))
{
	rotation_target = a;
	rotation_cb = cb;

/*
	Serial.print("motion_rotate angle:");
	Serial.print(a);
	Serial.print(" theta:");
	Serial.print(robot.theta);
	Serial.print(" pmax:");
	Serial.print(pmax);
	Serial.print(" pmin:");
	Serial.print(pmin);
	Serial.print("\n");
*/

	//print_rotation();

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
//	Serial.print(robot.theta);
	Serial.print(robot.theta/(2*PI)*360.0);
	Serial.print(", target:");
//	Serial.print(rotation_target);
	Serial.print(rotation_target/(2*PI)*360.0);
	Serial.print("\n");
}

void motion_rotation_update()
{
//	print_rotation();
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
			callback(&rotation_cb);
		}
	}
	else if(rotation_dir == R_CCW)
	{
		if(robot.theta > rotation_target)
		{
			callback(&rotation_cb);
		}
	}

}

float swing_min, swing_max, swing_pmin, swing_pmax;
void _swing_min_cb();
void _swing_max_cb();

void print_swing()
{
	Serial.print("Theta now:");
	Serial.print(robot.theta);
	Serial.print(" Swing [");
	Serial.print(swing_min);
	Serial.print(", ");
	Serial.print(swing_max);
	Serial.print("]\n");
}

void _swing_min_cb()
{
	Serial.print("swing min cb\n");
	if(swing_cb) swing_cb();
//	print_swing();
//	motion_rotate(swing_max, swing_pmin, swing_pmax, _swing_max_cb);
}

void _swing_max_cb()
{
	Serial.print("swing max cb\n");
	if(swing_cb) swing_cb();
//	print_swing();
//	motion_rotate(swing_min, swing_pmin, swing_pmax, _swing_min_cb);
}

void swing(float a, float delta, float pmin, float pmax, void (*cb)(void))
{
	swing_min = a - delta;
	swing_max = a + delta;
	swing_cb = cb;
	swing_pmin = pmin;
	swing_pmax = pmax;
//	print_swing();
	if(rotation_dir == R_CW)
		motion_rotate(swing_min, swing_pmin, swing_pmax, _swing_min_cb);
	else
		motion_rotate(swing_max, swing_pmin, swing_pmax, _swing_max_cb);
}

long wait_timeout = 0;

void motion_wait(long ms, void (*cb)(void))
{
	wait_timeout = millis() + ms;
	wait_cb = cb;
}

void motion_wait_update()
{
	if(!wait_cb) return;

	Serial.print("millis ");
	Serial.print(millis());
	Serial.print("timeout ");
	Serial.print(wait_timeout);
	Serial.print("\n");

	if(millis() > wait_timeout)
	{
		callback(&wait_cb);
	}
}

long stop_timeout;
char stop_counter;

#define STOP_MILLIS	50

void motion_stop(void (*cb)(void))
{
	stop_cb = cb;

	motors(0, 0);
	stop_timeout = millis() + STOP_MILLIS;
	stop_counter = robot.counter;
}

void motion_stop_update()
{
	if(robot.counter != stop_counter)
	{
		stop_counter = robot.counter;
		return;
	}

	if(millis() > stop_timeout) callback(&stop_cb);
}

double straight_end;
char straight_dir, straight_forever;
char straight_counter;
float straight_start_x, straight_start_y, straight_distance, straight_angle;
float straight_end_x, straight_end_y;

#define FORWARD		1
#define BACKWARD	-1

#define ERR_MAX	10
#define Kp 0.05 //200.0
#define KA -200
#define KX 2

void motion_straight_nothing() {}

void motion_straight(double distance, char forever, void (*cb)(void))
{
	if(distance == 0) return;

	straight_cb = cb;
	straight_start_x = robot.rx;
	straight_start_y = robot.ry;
	straight_forever = forever;

	if(forever) straight_cb = motion_straight_nothing;

	straight_distance = fabs(distance);
	straight_angle = robot.theta;
	straight_end_x = robot.rx + distance * cos(robot.theta);
	straight_end_y = robot.ry + distance * sin(robot.theta);
	
	straight_dir = distance > 0 ? FORWARD : BACKWARD;
	straight_counter = robot.counter;

	if(straight_dir == FORWARD)
	{
		motors(1, 1);
	}
	else
	{
		motors(-1, -1);
	}
}

float straight_error()
{
	int i;
	
	float prop, err_s, err_a, s, alpha, beta;

	s = pow(robot.rx - 100, 2);
	s += pow(robot.ry - 100, 2);
	s = sqrt(s);

	beta = asin((robot.rx - 100) / s);
	alpha = beta - robot.theta;

	err_s = - s * sin(alpha);
	err_a = straight_dir * (straight_angle - robot.theta);

	prop = Kp * (0*KX * err_s + KA * err_a);

	//TODO: Usar la variación en x para la correción, eliminando el
	//error acumulado.

/*
	Serial.print("s:");
	Serial.print(s);
	Serial.print("\terr_s:");
	Serial.print(err_s*KX);
	Serial.print("\terr_a:");
	Serial.print(err_a*KA);
	Serial.print("\tprop:");
	Serial.print(prop);
	Serial.print("\tbeta:");
	Serial.print(beta/(2*PI)*360);
	Serial.print("\n");
*/
/*
	fw_error[fw_error_i] = fw_pro;
	fw_error_i = (fw_error_i + 1) % ERR_MAX;
	fw_int = 0;

	for(i = 0; i < ERR_MAX; i++)
	{
		fw_int += fw_error[i];
	}

	ci = fw_int * Ki;
	cp = fw_pro * Kp;
	c = ci + cp;
*/

	return prop;
}

void motion_straight_update()
{
	if(robot.counter == straight_counter) return;
/*
	Serial.print("\tr:");
	Serial.print(robot.r);
	Serial.print(" end:");
	Serial.print(straight_end);
*/
	float s, c;
	s = pow(robot.rx - straight_start_x, 2);
	s += pow(robot.ry - straight_start_y, 2);
	s = sqrt(s);
	if(s > straight_distance && !straight_forever)
	{
		callback(&straight_cb);
		return;
	}

	c = straight_error();

	if(c > 1) c = 1;
	if(c < -1) c = -1;

	if(straight_dir == FORWARD)
	{
		if(c > 0)
			motors(1, 1-c);
		else if(c < 0)
			motors(1+c, 1);
	}
	else
	{
		if(c > 0)
			motors(-1, -1+c);
		else if(c < 0)
			motors(-1-c, -1);
	}

	straight_counter = robot.counter;
}

void callback(void (**cb)(void))
{
	if(!*cb) return;

	void (*tmp)(void) = *cb;
	(*cb) = NULL;
	tmp();
}

void motion_update()
{
	if(straight_cb)	motion_straight_update();
	if(wait_cb)	motion_wait_update();
	if(stop_cb)	motion_stop_update();
	if(rotation_cb)	motion_rotation_update();
}

