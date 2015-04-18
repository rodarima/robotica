#include "odometry/odometry.h"

#include <Arduino.h>
#include <serialmouse.h>
#include "pinout.h"

void odometry_init();
static void _odometry_update();
static void _mouse_update(unsigned char *buf);

struct robot_t robot;

void odometry_init()
{
	pinMode(PINMR0,OUTPUT);
	pinMode(PINMR1,OUTPUT);
	pinMode(PINML0,OUTPUT);
	pinMode(PINML1,OUTPUT);

	sm_init(_mouse_update);
}

void _odometry_update()
{
	float alpha = robot.vy / ROBOT_L;
	float phi = alpha / 2.0;

	if(alpha == 0)	/* Avanza en línea recta */
	{
		robot.x += robot.vx * cos(robot.theta);
		robot.y += robot.vx * sin(robot.theta);
	}
	else	/* Está girando */
	{
		float R_ = ROBOT_L * robot.vx / robot.vy;
		float rx, ry;

		if(alpha < 0) /* Gira en sentido horario */
		{
			rx = R_ * (1 - cos(alpha));
			ry = - R_ * sin(alpha);
		}
		else	/* Gira en sentido antihorario */
		{
			rx = - R_ * (1 - cos(alpha));
			ry = R_ * sin(alpha);
		}

		float mod_r = sqrt(rx*rx + ry*ry);
		float rx_ = mod_r * cos(phi);
		float ry_ = mod_r * sin(phi);

		//TODO: Invertir el signo de avance al retroceder.

		robot.x += rx_;
		robot.y += ry_;
	}

	robot.theta += alpha;
	robot.theta_deg = robot.theta * 360.0 / (2.0 * PI);
}

void _mouse_update(unsigned char *buf)
{
	char _mouse_vx, _mouse_vy;
	long t = micros();
	float _vx, _vy;
	float _ax, _ay;

	_mouse_vx = (char) (((buf[0] & 0x03) << 6) | (buf[1] & 0x3f));
	_mouse_vy = (char) (((buf[0] & 0x0C) << 4) | (buf[2] & 0x3f));

	_vx = (float) _mouse_vx / MOUSEMM;
	_vy = (float) _mouse_vy / MOUSEMM;

	/* Detectar errores midiendo la aceleración */
	_ax = robot.vx - _vx;
	_ay = robot.vy - _vy;

	if(abs(_ax) > 15.0 / MOUSEMM)
	{
		digitalWrite(13, 1);
		return;
	}

	robot.vx = _vx;
	robot.vy = _vy;

	robot.ax = _ax;
	robot.ay = _ay;

	robot.rx += _vx;
	robot.ry += _vy;

	/* Calculate odometry */
	_odometry_update();

	robot.counter++;

	robot.odometry_time = micros() - t;

	if(robot.odometry_time > 1000)
	{
		digitalWrite(13, 1);
		Serial.print("TIME UP ");
		Serial.println(robot.odometry_time);
	}

}
