#include <Arduino.h>

#define SLEEP 200

#define PINSNRT 11
#define PINSNRE 12

void setup()
{
	Serial.begin(115200);

	pinMode(PINSNRT, OUTPUT);
	pinMode(PINSNRE, INPUT);
}

int snr0;

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

void loop()
{
	snr0 = sonar();
	print_json();

	delay(60);
}
