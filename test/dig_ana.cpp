#include <Arduino.h>

#define PINA	0
#define PIND	5

void setup()
{
	Serial.begin(9600);
	pinMode(PIND, INPUT);
}

void loop()
{
	int analog = analogRead(PINA);
	int digital = digitalRead(PIND);

	Serial.print(analog);
	Serial.print(" ");
	Serial.print(digital);
	Serial.print("\n");
}
