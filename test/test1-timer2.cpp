#include <Arduino.h>
#include <timer2.h>

/* cs			1	2	3	4	5	6	7    */
/* prescaler		1	8	32	64	128	256	1024 */
/* max_time (us)      	16	128	512	1024	2048	4096	16384*/

uint8_t cnt[] =
{ 0, 255, 0, 255, 0, 255, 0, 255, 0, 255, 0, 255, 0, 255, 256-208};
uint8_t cs[] =
{ 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 4};

#define N sizeof(cnt)/sizeof(cnt[0])

static volatile unsigned long t;
static volatile unsigned long t1;
static volatile int i = 0;
static volatile long count = 1;
static volatile char waiting = 0;

static volatile long ttime[N] = {0};

void time_up()
{
	long d = micros() - t;
	timer2_stop();

	ttime[i] += d;

	Serial.print((double)ttime[i]/count);
	//Serial.print(t1);
	Serial.print("\t");
	waiting = 0;
}

void setup()
{
	pinMode(13, OUTPUT);
	digitalWrite(13, LOW);
	Serial.begin(115200);

}

void loop()
{
	while(1)
	{
		waiting = 1;
		t = micros();
		timer2_init(cnt[i], cs[i], time_up);
		t1 = micros() - t;
		t = micros();
		timer2_start();

		while(waiting);
		i++;
		if(i >= N)
		{
			i = 0;
			Serial.print("\n");
			count++;
		}
	}
}
