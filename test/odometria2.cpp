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

double x = 0, y = 0;
double global_x = 0, global_y = 0;
double dx, dy;
double angle = 0.0;
double angle_deg = 0.0;
unsigned char _counter;
long time;

//#define ROBOT_L	(97.5 - 0.25) //Casa
#define ROBOT_L	(97.5 + 0.10) //Suelo FIC
#define MOUSEMM 11.484

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

	if(count > 5000)
	{
		//count = -5000;
		count = 0;
		direction ^= 1;
		//Serial.print("!");
	}
	
	count++;
}



long ct = 0;
volatile long odometry_time = 0;

struct robot_t /* 18 bytes */
{
	double rx, ry;	// 8 bytes for fake position (mm) ?
	float vx, vy;	// 4 bytes for instantaneous velocity (mm/25ms)
	float ax, ay;	// 4 bytes for instantaneous acceleration
	double x, y;	// 8 bytes for global position (mm)
	float theta;	// 2 bytes for global angle (rad)
	unsigned char counter;	// 1 byte for counter
} robot;

void odometry()
{
	float alpha = robot.vy / ROBOT_L;
	float phi = alpha / 2.0;

	if(alpha == 0)	/* Avanza en línea recta */
	{
		robot.x += robot.vx * cos(robot.theta);
		robot.y += robot.vx * sin(robot.theta);
	}
	else	/* Está girando */
	{
		float R_ = fabs(ROBOT_L * robot.vx / robot.vy);
		float rx, ry;

		if(alpha < 0) /* Gira en sentido horario */
		{
			rx = R_ * (1 - cos(alpha));
			ry = - R_ * sin(alpha);
		}
		else	/* Gira en sentido antihorario */
		{
			rx = - R_ * (1 - cos(alpha));
			ry = R_ * sin(alpha);
		}

		float mod_r = sqrt(rx*rx + ry*ry);
		//TODO: Invertir el signo de avance al retroceder.
		if(robot.vx < 0.0) mod_r = - mod_r;

		float rx_ = mod_r * cos(phi);
		float ry_ = mod_r * sin(phi);

		
		robot.x += rx_;
		robot.y += ry_;
	}
	
	robot.theta += alpha;
	angle_deg += alpha / (2.0 * PI) * 360.0;
}

void mouse_update(unsigned char *buf)
{
	char _mouse_vx, _mouse_vy;
	long t = micros();
	float _vx, _vy;
	float _ax, _ay;

	_mouse_vx = (char) (((buf[0] & 0x03) << 6) | (buf[1] & 0x3f));
	_mouse_vy = (char) (((buf[0] & 0x0C) << 4) | (buf[2] & 0x3f));

	_vx = (float) _mouse_vx / MOUSEMM;
	_vy = (float) _mouse_vy / MOUSEMM;

	/* Detectar errores midiendo la aceleración */
	_ax = robot.vx - _vx;
	_ay = robot.vy - _vy;

	if(abs(_ax) > 15.0 / MOUSEMM)
	{
		digitalWrite(13, 1);
		return;
	}

	robot.vx = _vx;
	robot.vy = _vy;

	robot.ax = _ax;
	robot.ay = _ay;

	robot.rx += _vx;
	robot.ry += _vy;

	/* Calculate odometry */
	odometry();

	robot.counter++;

	odometry_time = micros() - t;

	if(odometry_time > 1000)
	{
		digitalWrite(13, 1);
		Serial.print("TIME UP ");
		Serial.println(odometry_time);
	}

}
void setup()
{
	Serial.begin(115200);
	//Serial.println("Encendido");

	pinMode(PINMR0,OUTPUT);
	pinMode(PINMR1,OUTPUT);
	pinMode(PINML0,OUTPUT);
	pinMode(PINML1,OUTPUT);

	sm_init(mouse_update);



	int v = 255;
	analogWrite(PINMR0, 0);
	analogWrite(PINMR1, v);
	analogWrite(PINML0, 0);
	analogWrite(PINML1, v);
	
}

void print_json()
{
        Serial.print("{");

        Serial.print("\"pos\":{");
        Serial.print("\"x\":");
        Serial.print(robot.x);
        Serial.print(", \"y\":");
        Serial.print(robot.y);
        Serial.print(", \"rx\":");
        Serial.print(robot.rx);
        Serial.print(", \"ry\":");
        Serial.print(robot.ry);
        Serial.print(", \"deg\":");
        Serial.print(angle_deg);
        Serial.print(", \"time\":");
        Serial.print(odometry_time);
        Serial.print("}");
        Serial.println("}");
}

void go_direction()

void loop()
{
	//test_motors();
	if(_counter != robot.counter)
	{
		_counter = robot.counter;
		if(robot.rx > 800)
		{
			int v = 255;
			analogWrite(PINMR0, 0);
			analogWrite(PINMR1, 0);
			analogWrite(PINML0, 0);
			analogWrite(PINML1, 0);
		}
		/*
		float abs_deg = fabs(angle_deg);
		if(abs_deg > 80 && abs_deg < 90)
		{
			float minv = 150.0;
			int v = (int) ((90.0 - abs_deg) / 10.0 * (256 - minv) + minv);
			Serial.println(v);
			if(v > 150)
			{
				analogWrite(PINMR0, 0);
				analogWrite(PINMR1, v);
				analogWrite(PINML0, v);
				analogWrite(PINML1, 0);
			}
			else
			{
				analogWrite(PINMR0, 0);
				analogWrite(PINMR1, 0);
				analogWrite(PINML0, 0);
				analogWrite(PINML1, 0);
			}
		}
		else if(abs_deg >= 90)
		{
			analogWrite(PINMR0, 0);
			analogWrite(PINMR1, 0);
			analogWrite(PINML0, 0);
			analogWrite(PINML1, 0);
		}*/
		/*
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
		}*/


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
		//time = micros() - time;
		print_json();
	}
	//delay(20);
}
