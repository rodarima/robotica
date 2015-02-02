/* ==========================================================
Project : Send Photo resistor values to computer
Author: ScottC
Created: 25th June 2011
Description: This sketch will make the arduino read Photo resistor
             values on analog pin A0. The analog readings will
             be dependent on the amount of light reaching the
             sensor. The Analog readings will be sent to the
             computer via the USB cable using Serial communication.
==============================================================
*/
#include <Arduino.h>

int led_azul = 13;
int photoRPin = 0; 
int minLight;          //Used to calibrate the readings
int maxLight;          //Used to calibrate the readings
int lightLevel;
int adjustedLightLevel;

#define SLEEP 100

void setup() {

	pinMode(led_azul, OUTPUT);

	Serial.begin(9600);

	//Setup the starting light level limits
	lightLevel=analogRead(photoRPin);
	minLight=lightLevel-20;
	maxLight=lightLevel;
}

void loop()
{
	int r0, r1, dif;
	
	digitalWrite(led_azul, LOW);
	
	delay(SLEEP);
	r0 = analogRead(photoRPin);
	
	digitalWrite(led_azul, HIGH);
	delay(SLEEP);
	
	r1 = analogRead(photoRPin);
	
	dif = r0 - r1;

	Serial.print("Real value: ");
	Serial.println(r0);
	if(dif >= 100)
	{
		Serial.print(dif);
		Serial.println(" Alerta de colisión");
	}
	else
	{
		Serial.println(dif);
	}


}
