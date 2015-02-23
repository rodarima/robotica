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

	return pulseIn(PINSNRE, HIGH, 8000);
}

void loop()
{
	ldr0_off = analogRead(PINLDR0);
	ldr1_off = analogRead(PINLDR1);
	
	Serial.print("ldr0_off:");
	Serial.print(ldr0_off);
	Serial.print("\tldr1_off:");
	Serial.print(ldr1_off);
	
	leds(1);
	delay(SLEEP);
	
	ldr0_on = analogRead(PINLDR0);
	ldr1_on = analogRead(PINLDR1);
	
	Serial.print("\tldr0_on:");
	Serial.print(ldr0_on);
	Serial.print("\tldr1_on:");
	Serial.print(ldr1_on);

	Serial.print("\tldr0_dif:");
	Serial.print(ldr0_off - ldr0_on);
	Serial.print("\tldr1_dif:");
	Serial.print(ldr1_off - ldr1_on);
	

	leds(0);
	delay(SLEEP);
	
	snr0 = sonar();
	Serial.print("\tsnr0:");
	Serial.print(snr0);

	
	Serial.print("\n");
}
