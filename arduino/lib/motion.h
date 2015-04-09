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
void rotate_cb(float a, float pmax, float pmin, void (*cb)(void));
void motion_wait(long ms, void (*cb)(void));
void motion_wait_update();


void swing(float a, float delta, float pmin, float pmax, void (*cb)(void));


#endif

