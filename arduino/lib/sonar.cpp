#include "sonar.h"

#include <Arduino.h>
#include <pinout.h>

/* Sonar library */

unsigned long sonar_last_t;
unsigned long sonar_start_t;
volatile int sonar_inc_t;

void sonar_pulse();
void sonar_pulse_start();
void sonar_pulse_end();
void sonar_update();

void sonar_pulse()
{
	sonar_last_t = micros();

	attachInterrupt(INT_SNR0, sonar_pulse_start, RISING);

	digitalWrite(PIN_SNR0S, 0);
	delayMicroseconds(5);
	digitalWrite(PIN_SNR0S, 1);
	delayMicroseconds(10);
}

void sonar_pulse_end()
{
	sonar_inc_t = micros() - sonar_start_t;
	//sonar_pulse();
}
void sonar_pulse_start()
{
	sonar_start_t = micros();
	attachInterrupt(INT_SNR0, sonar_pulse_end, FALLING);
}

void sonar_init()
{
	pinMode(PIN_SNR0S, OUTPUT);
	pinMode(PIN_SNR0R, INPUT);

	sonar_inc_t = -1;
	sonar_pulse();
}

void sonar_update(volatile int *snr)
{
	unsigned long t = micros();
	if(t - sonar_last_t > SONAR_TIMEOUT)
	{
		sonar_inc_t = -1;
		sonar_pulse();
	}
	*snr = sonar_inc_t;
}

/* END Sonar library */

