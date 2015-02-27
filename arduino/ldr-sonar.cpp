#include <Arduino.h>

#define SLEEP 200

#define PINLDR0	0
#define PINLDR1	1

#define PINLEDR	4
#define PINLEDG	7
#define PINLEDB	8

#define PINSNRT 11
#define PINSNRE 12

void setup()
{
	Serial.begin(115200);
	pinMode(PINLEDR, OUTPUT);
	pinMode(PINLEDG, OUTPUT);
	pinMode(PINLEDB, OUTPUT);

	pinMode(PINSNRT, OUTPUT);
	pinMode(PINSNRE, INPUT);
}

int ldr0_off, ldr1_off, ldr0_on, ldr1_on, snr0;

void leds(int val)
{
	digitalWrite(PINLEDR, val);
	digitalWrite(PINLEDG, val);
	digitalWrite(PINLEDB, val);
}

int sonar()
{
	digitalWrite(PINSNRT, 0);
	delayMicroseconds(5);
	digitalWrite(PINSNRT, 1);
	delayMicroseconds(10);

	return pulseIn(PINSNRE, HIGH, 20000);
}

void print_json()
{
	Serial.print("{\"snr0\":");
	Serial.print(snr0);
	Serial.print("}\n");
}

void loop()
{
	snr0 = sonar();
	print_json();
	delay(50);
}
