#include "odometry.h"

#include <Arduino.h>
#include <serialmouse.h>
#include <pinout.h>

void odometry_init();
static void _mouse_update(unsigned char *buf);

struct robot_t robot;

void odometry_init()
{
	pinMode(PIN_MTR0,OUTPUT);
	pinMode(PIN_MTR1,OUTPUT);
	pinMode(PIN_MTL0,OUTPUT);
	pinMode(PIN_MTL1,OUTPUT);

	sm_init(_mouse_update);
	robot.mx = 0;
	robot.my = 0;
	robot.rx = 0;
	robot.ry = 0;
	robot.theta = 0;
	robot.r = 0;
}

static void update_odometry(struct robot_t *robot, float mx, float my)
{
	float r, vx, vy, alpha, phi;
	// Pequeña corrección para ajustar la dirección del mouse
	float epsilon = robot->vy >= 0 ? 0.0001 : -0.0001;

	alpha = mx / ROBOT_L + epsilon;
	phi = (PI + alpha) / 2.0;

	if(mx == 0.0)
	{
		r = my;
	}
	else
	{
		r = 2.0 * ROBOT_L * my / mx * sin(mx / (2.0 * ROBOT_L));
	}

	robot->r += fabs(r);
	vx = r * cos(robot->theta + phi);
	vy = r * sin(robot->theta + phi);

	robot->vx = vx;
	robot->vy = vy;

	robot->rx += vx;
	robot->ry += vy;

	robot->theta += alpha;

	//TODO: Angulo entre 0 y 2 PI ?
	//while(robot->theta > 2.0*PI) robot->theta -= 2.0 * PI;
	//while(robot->theta < 0) robot->theta += 2.0 * PI;
}

//void _odometry_update_caca()
//{
//	float alpha = robot.vy / ROBOT_L;
//	float phi = alpha / 2.0;
//
//	if(alpha == 0)	/* Avanza en línea recta */
//	{
//		robot.x += robot.vx * cos(robot.theta);
//		robot.y += robot.vx * sin(robot.theta);
//	}
//	else	/* Está girando */
//	{
//		float R_ = ROBOT_L * robot.vx / robot.vy;
//		float rx, ry;
//
//		if(alpha < 0) /* Gira en sentido horario */
//		{
//			rx = R_ * (1 - cos(alpha));
//			ry = - R_ * sin(alpha);
//		}
//		else	/* Gira en sentido antihorario */
//		{
//			rx = - R_ * (1 - cos(alpha));
//			ry = R_ * sin(alpha);
//		}
//
//		float mod_r = sqrt(rx*rx + ry*ry);
//		float rx_ = mod_r * cos(phi);
//		float ry_ = mod_r * sin(phi);
//
//		//TODO: Invertir el signo de avance al retroceder.
//
//		robot.x += rx_;
//		robot.y += ry_;
//	}
//
//	robot.theta += alpha;
//	robot.theta_deg = robot.theta * 360.0 / (2.0 * PI);
//}

void _mouse_update(unsigned char *buf)
{
	char h, v;
	long t = micros();
	float mx, my;

	h = (char) (((buf[0] & 0x03) << 6) | (buf[1] & 0x3f));
	v = (char) (((buf[0] & 0x0C) << 4) | (buf[2] & 0x3f));

	mx = (float) v / MOUSEMM;
	my = (float) h / MOUSEMM;

	robot.mx += mx;
	robot.my += my;

	/* Calculate odometry */
	update_odometry(&robot, mx, my);

	robot.counter++;

	robot.odometry_time = micros() - t;
//	Serial.print("U");

	if(robot.odometry_time > 1000)
	{
		digitalWrite(13, 1);
//		Serial.print("TIME UP ");
//		Serial.println(robot.odometry_time);
	}

}
