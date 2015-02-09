#ifndef _TIMER2_H_
#define _TIMER2_H_

/* Initialize timer2, but don't start the timer2
 * Return:
 * -1: If 'us' is too big.
 *  0: Sucess.
*/
void timer2_init(unsigned char tcnt2, unsigned char cs, void (*f)(void));


void timer2_start();
void timer2_stop();

void timer2_pause();
void timer2_continue();

#endif /* _TIMER2_H_ */
