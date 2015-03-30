#ifndef _SONAR_H_
#define _SONAR_H_

#define SONAR_TIMEOUT	20000

void sonar_init();
void sonar_update(volatile int *snr);

#endif
