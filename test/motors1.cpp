#include <Arduino.h>
#include <serialmouse.h>

char direction = 0;
long count = 0;

#define PINMR0	9
#define PINMR1	10
#define PINML0	5
#define PINML1	6

#define PINSE	11
#define PINSR	12

int ml0 = 255, ml1 = 255, mr0 = 255, mr1 = 255;

void test_motors()
{
	if(count < 0)
	{
		digitalWrite(PINMR0, 0);
		digitalWrite(PINMR1, 0);
		digitalWrite(PINML0, 0);
		digitalWrite(PINML1, 0);
	}
	else
	{
		if(direction)
		{
			analogWrite(PINMR0, 0);
			analogWrite(PINMR1, mr1);
			analogWrite(PINML0, 0);
			analogWrite(PINML1, ml1);
		}
		else
		{
			analogWrite(PINMR0, ml0);
			analogWrite(PINMR1, 0);
			analogWrite(PINML0, mr0);
			analogWrite(PINML1, 0);
		}
	}

	if(count > 50000)
	{
		count = -5000;
		//count = 0;
		direction ^= 1;
		//Serial.print("!");
	}
	
	count++;
}

void setup()
{
	Serial.begin(115200);
	//Serial.println("Encendido");

	pinMode(PINMR0,OUTPUT);
	pinMode(PINMR1,OUTPUT);
	pinMode(PINML0,OUTPUT);
	pinMode(PINML1,OUTPUT);

	sm_init();
}

double x = 0, y = 0;
double dx, dy;

#define MOUSEMM 11.484

void print_json()
{
        Serial.print("{");

        Serial.print("\"pos\":{");
        Serial.print("\"x\":");
        Serial.print(x);
        Serial.print(", \"y\":");
        Serial.print(y);
        Serial.print(", \"dx\":");
        Serial.print(dx);
        Serial.print(", \"dy\":");
        Serial.print(dy);
        Serial.print(", \"ml0\":");
        Serial.print(ml0);
        Serial.print(", \"mr0\":");
        Serial.print(mr0);
        Serial.print(", \"ml1\":");
        Serial.print(ml1);
        Serial.print(", \"mr1\":");
        Serial.print(mr1);
        Serial.print("}");
        Serial.println("}");
}


long ct = 0;

void loop()
{
	test_motors();
	if(x != (double) mousex / MOUSEMM || y != (double) mousey / MOUSEMM)
	{
		dx = (double) mousex / MOUSEMM - x;
		dy = (double) mousey / MOUSEMM - y;
		x = (double) mousex / MOUSEMM;
		y = (double) mousey / MOUSEMM;

		if(y > 0.1)
		{
			if(direction)
			{
				//ml1 = ml1 >= 255 ? 255 : ml1+1;
				//mr1 = mr1 <= 200 ? 200 : mr1-1;
				ml1 = 255;
				mr1 = 230;
			}
			else
			{
				//ml0 = ml0  >= 255 ? 255 : ml0+1;
				//mr0 = mr0 <= 200 ? 200 : mr0-1;
				ml0 = 255;
				mr0 = 220;
			}
		}
		else if(y < 0.1)
		{
			if(direction)
			{
				//mr1 = mr1 >= 255 ? 255 : mr1+1;
				//ml1 = ml1 <= 200 ? 200 : ml1-1;
				mr1 = 255;
				ml1 = 220;
			}
			else
			{
				//mr0 = mr0 >= 255 ? 255 : mr0+1;
				//ml0 = ml0 <= 200 ? 200 : ml0-1;
				mr0 = 255;
				ml0 = 200;
			}
		}


		/*
		Serial.write(buf[0]);
		Serial.write(buf[1]);
		Serial.write(buf[2]);
		*/
		/*
		Serial.print(buf[0], HEX);
		Serial.print(" ");
		Serial.print(buf[1], HEX);
		Serial.print(" ");
		Serial.print(buf[2], HEX);
		Serial.print("\n");
		*/
		/*
		Serial.print("x = ");
		Serial.print((double)x/MOUSEMM);
		Serial.print("\ty = ");
		Serial.print((double)y/MOUSEMM);
		Serial.print("\tdx = ");
		Serial.print(dx);
		Serial.print("\tdy = ");
		Serial.println(dy);
		*/
		/*
		if(dx > 1 || dx < -1)
		{
			pinMode(13, OUTPUT);
			digitalWrite(13, 1);
			//Serial.println("ERROR EN LA MEDIDA");
			digitalWrite(PINMR0, 0);
			digitalWrite(PINMR1, 0);
			digitalWrite(PINML0, 0);
			digitalWrite(PINML1, 0);
			while(1);
		}
		*/
		print_json();
	}
}
