#ifndef _MOTION_H_
#define _MOTION_H_

void motion_init();
void motion_rotate(float theta);
void motor_right(float p);
void motor_left(float p);
void motors(float r, float l);
void motion_rotate_delta(float theta_delta);
void motion_update();
void motion_set_velocity(float v);
void motion_move(float d);

void rotation_update();

#endif

