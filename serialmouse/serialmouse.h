#ifndef _SERIALMOUSE_H_
#define _SERIALMOUSE_H_

#define PININT		0
#define PINMOUSE	2

void sm_init();
volatile extern long mousex, mousey;

#endif
