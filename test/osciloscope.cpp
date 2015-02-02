#include <Arduino.h>

#define PINA	0

void setup()
{
	Serial.begin(115200);
}

int value;
unsigned long time;

void read_analog()
{
	time = micros();
	value = analogRead(PINA);
}

void print_json()
{
	Serial.write(0xff);
	Serial.write((time >> 8) & 0xff);
	Serial.write(time & 0xff);
	Serial.write((value >> 8) & 0xff);
	Serial.write(value & 0xff);
}
	

void loop()
{
	read_analog();
	print_json();
}
