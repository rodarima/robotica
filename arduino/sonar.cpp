#include <Arduino.h>

#define SLEEP 200

#define PINSNRT 11
#define PINSNRE 12

#define PIN_SONAR_SEND	11
#define PIN_SONAR_RECV	3
#define INT_SONAR_RECV	1
#define SONAR_TIMEOUT	20000

/* Sonar library */

unsigned long sonar_last_t;
unsigned long sonar_start_t;
unsigned int sonar_inc_t;

void sonar_pulse();
void sonar_pulse_start();
void sonar_pulse_end();
void sonar_update();

void sonar_pulse()
{
	sonar_last_t = micros();

	attachInterrupt(INT_SONAR_RECV, sonar_pulse_start, RISING);

	digitalWrite(PIN_SONAR_SEND, 0);
	delayMicroseconds(5);
	digitalWrite(PIN_SONAR_SEND, 1);
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
	attachInterrupt(INT_SONAR_RECV, sonar_pulse_end, FALLING);
}

void sonar_init()
{
	sonar_inc_t = 0;
	sonar_pulse();
}

void sonar_update()
{
	unsigned long t = micros();
	if(t - sonar_last_t > SONAR_TIMEOUT)
	{
		//sonar_inc_t = 0;
		sonar_pulse();
	}
}

/* END Sonar library */

void setup()
{
	Serial.begin(115200);

	pinMode(PIN_SONAR_SEND, OUTPUT);
	pinMode(PIN_SONAR_RECV, INPUT);
	sonar_init();
}

double snr0;

int sonar()
{
	digitalWrite(PINSNRT, 0);
	delayMicroseconds(5);
	digitalWrite(PINSNRT, 1);
	delayMicroseconds(10);

	return pulseIn(PINSNRE, HIGH, 50000);
}


void print_json()
{
	Serial.print("{\"snr0\":");
	Serial.print(snr0);
	Serial.print("}\n");
}

#define N 100000
double mt = 0;
double i = 0;

void loop()
{
	mt += sonar_inc_t;
	i++;

	//snr0 = sonar_inc_t;
	//print_json();
	if(i>=N)
	{
		snr0 = mt / i;
		print_json();
		i = 0;
		mt = 0;
	}


	sonar_update();

	//delay(60);
}
