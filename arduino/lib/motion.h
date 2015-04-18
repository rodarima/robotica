#ifndef _MOTION_H_
#define _MOTION_H_


/* Manipular el voltaje de los motores */
void motors(float r, float l);

/* Giros */
void motion_rotate(float a, float pmin, float pmax, void (*cb)(void));
void motion_rotation_update();

/* Balanceo */
void swing(float a, float delta, float pmin, float pmax, void (*cb)(void));
/* No tiene update */

/* Esperas */
void motion_wait(long ms, void (*cb)(void));
void motion_wait_update();

/* Detención */
void motion_stop(void (*cb)(void));
void motion_stop_update();

/* Avanzar en línea recta */
void motion_straight(double distance, void (*cb)(void));
void motion_straight_update();

/* Actualizar todas las funciones (callback) */
void motion_update();

#endif

