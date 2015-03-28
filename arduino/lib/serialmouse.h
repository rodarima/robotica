#ifndef _SERIALMOUSE_H_
#define _SERIALMOUSE_H_

#define PININT		0
//#define PINMOUSE	7	//For analog compare
#define PINMOUSE	2

void sm_init(void (*f)(unsigned char *));
volatile extern long mouse_rx, mouse_ry;
volatile extern char mouse_vx, mouse_vy;
volatile extern int mouse_ax, mouse_ay;

#endif
