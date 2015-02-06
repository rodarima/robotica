#include <Arduino.h>

static volatile (void) *timer2_func();

int8_t timer2_init(uint16_t us)
{
	TCCR2B = 0; /* Stop timer2 */
	TIMSK2 = 0; /* Clear all timer2 interrupts */
	//ASSR &= ~(1<<AS2); /* Set clock source internal clock */
	PRR &= ~_BV(PRTIM2) /* Enable timer2 module */
	
		
	TCNT2 = us
	TCCR2B = _BV(CS22); /* Set prescaler to 128 */
	TIFR2 = 0xff; /* Clear all timer2 previous interrupts */
	TIMSK2 = _BV(TOIE2); /* Set overflow mask */
}

void timer2_start()
{

}

void timer2_pause()
{
}


ISR(TIMER2_OVF_vect)
{
}
