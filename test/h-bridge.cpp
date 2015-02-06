#include <Arduino.h>
#include <SoftwareSerial.h>
//#include <AltSoftSerial.h>

#define PINMR0	9
#define PINMR1	3
#define PINML0	5
#define PINML1	6

SoftwareSerial sm(4, 2, true);
//AltSoftSerial sm(4, 2, true);
long x=0, y=0;

void setup()
{
	Serial.begin(115200);
	sm.begin(1200);
	pinMode(PINMR0,OUTPUT);
	pinMode(PINMR1,OUTPUT);
	pinMode(PINML0,OUTPUT);
	pinMode(PINML1,OUTPUT);
}

void print_position()
{
	Serial.write(sm.read());
	/*char c[3];
	int i;
	for(i=0; i<3; i++)
	{
		c[i] = sm.read();
	}

	x +=   (char) ((c[0] << 6) | (c[1] & 0x7f));
	y += - (char) ((c[0] << 4) | (c[2] & 0x7f));
	
	Serial.print(c[0], HEX);
	Serial.print(" ");
	Serial.print(c[1], HEX);
	Serial.print(" ");
	Serial.print(c[2], HEX);
	Serial.print(" ");
	Serial.print("x = ");
	Serial.print(x);
	Serial.print(", y = ");
	Serial.println(y);*/
}

void loop()
{
	int i;
	//analogWrite(PINMR0, 128);
	digitalWrite(PINMR0, HIGH);
	digitalWrite(PINMR1, LOW);
	digitalWrite(PINML0, HIGH);
	digitalWrite(PINML1, LOW);
	for(i = 0; i < 50; i++)
	{
		delay(10);
		if(sm.available())
			print_position();
	}

	digitalWrite(PINMR0, LOW);
	digitalWrite(PINMR1, HIGH);
	digitalWrite(PINML0, LOW);
	digitalWrite(PINML1, HIGH);
	for(i = 0; i < 50; i++)
	{
		delay(10);
		if(sm.available())
			print_position();
	}
}
