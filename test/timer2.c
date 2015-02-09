#include <Arduino.h>

static volatile (void) *timer2_func() = NULL;

#define ARRAY_SIZE(x) (sizeof(x)/sizeof(x[0]))

/* Prescaler bit shift values for 16 MHZ */
char t_displ[] = { 4,  7,  9, 10, 11, 12, 14 };

int calculate_prescaler(unsigned long us)
{
	unsigned long presc;
	double max_time;
	int i;

	for(i = 0; i < ARRAY_SIZE(t_displ), i++)
	{
		max_time = ((unsigned long)1L) << t_displ[i];
		if(us < max_time)
		{
			return i + 1;
		}
	}
	return -1;
}

unsigned char calculate_timer(unsigned long us, char cs)
{
	int prescaler = 1 << (t_displ[cs - 1] - 4);
	return (unsigned char) 256 - us * (F_CPU / 1000000) / prescaler;
}

char timer2_init(unsigned long us, (void)*f())
{
	char cs = calculate_prescaler(us);
	if(cs < 0) return -1;

	unsigned char tcnt = calculate_timer(us, cs);

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

	timer2_func = f;
}

void timer2_start()
{
	/* Section 17.11.3: Set the timer2 value */
	TCNT2 = timer2_cnt;
	
	/* Section 17.11.6: Enable timer2 overflow mask */
	TIMSK2 |= _BV(TOIE2)
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
