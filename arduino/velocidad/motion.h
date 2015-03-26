#ifndef _MOTION_H_
#define _MOTION_H_

void motion_init();
void motion_rotate(float theta);
void motion_rotate_delta(float theta_delta);
void motion_update();
void motion_set_velocity(float v);
void motion_move(float d);

#endif

