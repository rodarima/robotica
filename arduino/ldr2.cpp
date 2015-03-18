#include <Arduino.h>

#define PINLDR0	0
#define PINLDR1	1


void setup()
{
	Serial.begin(115200);
}

int ldr0, ldr1;
double ldr0m = 0, ldr1m = 0;
unsigned long i = 0;

#define N 1000

void loop()
{
	ldr0 = analogRead(PINLDR0);
	ldr1 = analogRead(PINLDR1);

	ldr0m += ldr0;
	ldr1m += ldr1;
	i++;

	if(i > N)
	{
		ldr0m/=i;
		ldr1m/=i;
		i = 0;
		Serial.print("ldr0:");
		Serial.print(ldr0m);
		Serial.print("\tldr1:");
		Serial.print(ldr1m);
		Serial.print("\n");
		ldr0m=0;
		ldr1m=0;
		
	}
	
}
