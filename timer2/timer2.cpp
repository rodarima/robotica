#include "timer2.h"

#include <Arduino.h>

static volatile void (*timer2_func)(void) = NULL;

#define ARRAY_SIZE(x) (sizeof(x)/sizeof(x[0]))

/* Prescaler bit shift values for 16 MHZ */
char t_displ[] = 
		{	4,	7,	9,	10,	11,	12,	14 };
/* cs			1	2	3	4	5	6	7    */
/* prescaler		1	8	32	64	128	256	1024 */
/* max_time (us)      	16	128	512	1024	2048	4096	16384*/

volatile char timer2_cs;
volatile unsigned char timer2_cnt;

int calculate_prescaler(unsigned int us)
{
	unsigned int presc;
	double max_time;
	int i;

	for(i = 0; i < ARRAY_SIZE(t_displ); i++)
	{
		max_time = ((unsigned int)1L) << t_displ[i];
		if(us < max_time)
		{
			return i + 1;
		}
	}
	return 7;
}

unsigned char calculate_timer(unsigned int us, char cs)
{
	int prescaler = 1 << (t_displ[cs - 1] - 4);
	return (unsigned char) 256 - us * (F_CPU / 1000000) / prescaler;
}

void timer2_init(unsigned char tcnt2, unsigned char cs, void (*f)(void))
{
	//timer2_cs = calculate_prescaler(us);
	//timer2_cnt = calculate_timer(us, timer2_cs);

	timer2_cnt = tcnt2;
	timer2_cs = cs;
	timer2_func = (volatile void (*)(void)) f;

	/* Section 17.11.6: Disable overflow interrupt */
	TIMSK2 &= ~_BV(TOIE2);

	/* Table 17-8: Set normal mode using WGM* */
	TCCR2A &= ~(_BV(WGM20) | _BV(WGM21));
	TCCR2B &= ~_BV(WGM22);

	/* Table 17-9: Clear prescaler (stop timer2) */
	TCCR2B &= ~(_BV(CS22) | _BV(CS21) | _BV(CS20));

	/* Table 17-9: Set the calculated prescaler */
	TCCR2B |= timer2_cs;
	
	/* Section 17.11.7: Clear previous interrupt */
	TIFR2 |= _BV(TOV2);

}

void timer2_start()
{
	/* Section 17.11.3: Set the timer2 value */
	TCNT2 = timer2_cnt;
	
	/* Section 17.11.7: Clear previous interrupt */
	TIFR2 |= _BV(TOV2);
	
	/* Section 17.11.6: Enable timer2 overflow mask */
	TIMSK2 |= _BV(TOIE2);
}

void timer2_stop()
{
	/* Section 17.11.6: Disable overflow interrupt */
	TIMSK2 &= ~_BV(TOIE2);
}

void timer2_pause()
{
	/* Table 17-9: Clear prescaler */
	TCCR2B &= ~(_BV(CS22) | _BV(CS21) | _BV(CS20));
}

void timer2_continue()
{
	/* Table 17-9: Set the calculated prescaler */
	TCCR2B |= timer2_cs;
}

ISR(TIMER2_OVF_vect)
{
	if(timer2_func)	timer2_func();
}
