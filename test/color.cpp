#include <Arduino.h>

#define PIN_LDR	0
#define PIN_R	12
#define PIN_G	11
#define PIN_B	13
//#define SLEEP	500
#define DEBUG 0

void setup()
{
	pinMode(PIN_R, OUTPUT);
	pinMode(PIN_G, OUTPUT);
	pinMode(PIN_B, OUTPUT);

	Serial.begin(9600);
}

int coff, cr, cg, cb, don, doff;

void read_color(int SLEEP)
{
	delay(SLEEP);
	coff = analogRead(PIN_LDR);
	delay(SLEEP);
	
	// Red
	digitalWrite(PIN_R, HIGH);
	delay(SLEEP);
	cr = analogRead(PIN_LDR);
	delay(SLEEP);
	digitalWrite(PIN_R, LOW);
	delay(SLEEP);
	
	// Green
	digitalWrite(PIN_G, HIGH);
	delay(SLEEP);
	cg = analogRead(PIN_LDR);
	delay(SLEEP);
	digitalWrite(PIN_G, LOW);
	delay(SLEEP);
	
	// Blue
	digitalWrite(PIN_B, HIGH);
	delay(SLEEP);
	cb = analogRead(PIN_LDR);
	delay(SLEEP);
	digitalWrite(PIN_B, LOW);

}

void read_distance(int SLEEP)
{
	delay(SLEEP);
	doff = analogRead(PIN_LDR);
	delay(SLEEP);
	
	digitalWrite(PIN_R, HIGH);
	digitalWrite(PIN_G, HIGH);
	digitalWrite(PIN_B, HIGH);
	delay(SLEEP);
	don = analogRead(PIN_LDR);
	delay(SLEEP);
	digitalWrite(PIN_R, LOW);
	digitalWrite(PIN_G, LOW);
	digitalWrite(PIN_B, LOW);
	delay(SLEEP);

}

void print_json()
{
	Serial.print("{");

	Serial.print("\"color\":{");
	Serial.print("\"off\":");
	Serial.print(coff);
	Serial.print(", \"r\":");
	Serial.print(cr);
	Serial.print(", \"g\":");
	Serial.print(cg);
	Serial.print(", \"b\":");
	Serial.print(cb);
	Serial.print("}");
	
	Serial.print(", ");
	Serial.print("\"distance\":{");
	Serial.print("\"off\":");
	Serial.print(doff);
	Serial.print(", \"on\":");
	Serial.print(don);
	Serial.print("}");

	Serial.println("}");
}
	

void loop()
{
	read_color(100);
	read_distance(100);
	print_json();

}
