#include <Arduino.h>
#include <timer2.h>

static volatile unsigned long rx_time;

void store_byte(uint8_t byte);
int read_byte(uint8_t *buf);
void handle_bit();
void handle_interrupt();

void err()
{
	digitalWrite(13, HIGH);
}

void handle_bit()
{
	timer2_stop();
	unsigned long diff_time = micros() - rx_time;

	
	//Serial.print("TIME:");
	//Serial.print(F_CPU);
	//Serial.print(" ");
	Serial.print(diff_time);
	//Serial.print(" ");
	//Serial.print(TCNT2);
	//Serial.print(" ");
	//Serial.print(TIFR2, HEX);
	//Serial.print(" ");
	//Serial.print(cancer);

	//Serial.print(" TCNT1:");
	//Serial.print((unsigned long)TCNT1);
	//Serial.print(" TIFR1:");
	//Serial.print(TIFR1, HEX);
	Serial.print("\n");
	if((diff_time > 1249+20) ||
		(diff_time < 1249-20))
		err();
	
	attachInterrupt(0, handle_interrupt, CHANGE);
}

void handle_interrupt()
{
	detachInterrupt(0);
	rx_time = micros();
	timer2_start();
}

void setup()  
{
	micros();
	pinMode(13, OUTPUT);

	digitalWrite(13, LOW);
	Serial.begin(115200);
	timer2_init(1250, handle_bit);

	attachInterrupt(0, handle_interrupt, CHANGE);
}

void loop() {}
