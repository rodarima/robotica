#ifndef _ODOMETRY_H_
#define _ODOMETRY_H_

#define ROBOT_L	(97.5 - 0.25)
#define MOUSEMM 11.484

struct robot_t
{
	double rx, ry;	// 8 bytes for fake position (mm) ?
	float vx, vy;	// 4 bytes for instantaneous velocity (mm/25ms)
	float ax, ay;	// 4 bytes for instantaneous acceleration
	double sx, sy;	// 8 bytes for global position (mm)
	float theta;	// 2 bytes for global angle (rad)
	float theta_deg;	// 2 bytes for global angle (deg)
	unsigned char counter;	// 1 byte for counter
	int odometry_time;
};


void odometry_init();
extern struct robot_t robot;

#endif
