#include <Arduino.h>
#include <timer2.h>

#define SPEED		1200
#define WAIT		1000000./1200.


#define RX_BUFFER_MAX	100

/*
 * rx_buffer [ ... | x | x | x |   | ... ]
 *                   ^           ^
 *                   |           |
 *                   |           rx_tail
 *                   rx_head
 */
static volatile uint8_t rx_buffer[RX_BUFFER_MAX];
static volatile uint8_t rx_head; //First byte position for read
static volatile uint8_t rx_tail; //Next available position for store byte
static volatile uint8_t rx_actual_byte;
static volatile uint8_t rx_actual_bit;
static volatile uint8_t rx_overflow;
static volatile uint8_t rx_pin;
static volatile int8_t rx_mode;
static volatile uint8_t rx_inverted_logic;
static volatile uint8_t go0 = 0;
static volatile uint8_t three = 0;
static volatile uint16_t rx_time;
static volatile uint8_t rx_start_count;


void store_byte(uint8_t byte);
int read_byte(uint8_t *buf);
void handle_bit();
void handle_interrupt();

void err()
{
	digitalWrite(13, HIGH);
}
volatile char togf = 0;
void tog()
{
	if(togf)
	{
		digitalWrite(13, HIGH);
	}
	else
	{
		digitalWrite(13, LOW);
	}
	togf = togf ^ 1;
}

void store_byte(uint8_t byte)
{
	//Check if is there room for new byte
	uint8_t rx_next_tail = (rx_tail + 1) % RX_BUFFER_MAX;

	if(rx_next_tail == rx_head)
	{
		//No space left
		rx_overflow = 1;
		return;
	}

	// Write the new byte in the last tail position
	rx_buffer[rx_tail] = byte;

	// Update the next tail position
	rx_tail = rx_next_tail;
}

int read_byte(uint8_t *buf)
{
	//Check for new data
	if(rx_head == rx_tail)
	{
		// There is no data to be read
		return -1;
	}

	*buf = rx_buffer[rx_head];
	rx_head = (rx_head + 1) % RX_BUFFER_MAX;
	return 0;
}

void handle_last_bit()
{

	/*
	uint16_t diff_time = micros() - rx_time;

	if((diff_time > 7916+15) || 
		(diff_time < 7916)) err();
	*/
	
	timer2_stop();
	//uint16_t diff_time = micros() - rx_time;

	//Serial.println(diff_time);

	//uint8_t bit = digitalRead(2);
	//Serial.println(bit);
	
	attachInterrupt(0, handle_interrupt, RISING);
}

void handle_bit()
{
	uint16_t diff_time = micros() - rx_time;
	
	if((rx_actual_bit == 0) && (
		(diff_time > 1249+20) ||
		(diff_time < 1249-20)))
	{
		err();
	}
	
	/*
	if((rx_actual_bit == 6) && (
		//(diff_time > 6249+100) || 
		(diff_time < 6249-100-833*2)))
		err();
	//else if(rx_actual_bit == 6)
	//	digitalWrite(13, LOW);
	*/

	uint8_t bit = digitalRead(2);

	rx_actual_byte = (rx_actual_byte >> 1) | (bit<<6);

	// First call?
	if(rx_actual_bit != 6)
	{
		//Set the timer to 833.33 us
		//timer2_init(833-300, handle_bit);
		timer2_init(48+4, 4, handle_bit);
		timer2_start();
	}
	// End of data bits ?
	else if(rx_actual_bit == 6)
	{
		//Then turn off the timer, and write the new byte
		store_byte((rx_inverted_logic ?
				(~rx_actual_byte)
				: rx_actual_byte) & 0x7f);
	
		//timer2_init(833*2, handle_last_bit);
		timer2_init(48+4, 5, handle_last_bit);
		//timer2_init(48+4, 4, handle_last_bit);
		//timer2_init(0, 5, handle_last_bit);
		timer2_start();
	}

	rx_actual_bit++;
	//Serial.println(diff_time);
	

	sei();
}

void handle_interrupt()
{
	detachInterrupt(0);

	if(rx_start_count < 4)
	{
		if(!digitalRead(2))
		{
			err();
			timer2_stop();
			rx_start_count = 0;
			
			attachInterrupt(0, handle_interrupt, RISING);
			return;
		}
		timer2_init(25, 5, handle_interrupt);
		rx_start_count++;
	}
	
	
        //EIMSK &= ~(1<<INT0);
	
	/*
	if((rx_mode == RISING) && (rx_actual_bit == 7))
	{
		uint16_t diff_time = micros() - rx_time;
		if((diff_time < 7000) ||
			(diff_time > 6600)) err();
	}
	*/
	

	rx_time = micros();

	//timer2_init(100, 5, handle_bit);
	//timer2_init(100, 5, handle_bit);
	//timer2_start();


	// Detect logic mode
	/*
	if(rx_mode == -1)
	{
		if(bit)
		{
			rx_mode = RISING;
			rx_inverted_logic = 1;
		}
		else
		{
			rx_mode = FALLING;
			rx_inverted_logic = 0;
		}
	}
	*/


	rx_actual_byte = 0;
	rx_actual_bit = 0;
	handle_bit();
}

uint8_t available()
{
	if(rx_head == rx_tail) return 0;
	if(rx_head > rx_tail)
	{
		return RX_BUFFER_MAX - (rx_head - rx_tail);
	}
	return rx_tail - rx_head;
}

uint8_t overflow()
{
	return rx_overflow;
}

void myinit(int pin)
{
	rx_pin = pin;
	rx_overflow = 0;
	rx_head = 0;
	rx_tail = 0;
	rx_mode = -1;
	rx_start_count = 0;

	pinMode(2, INPUT);
	delay(200);
	attachInterrupt(0, handle_interrupt, RISING);
}

#define PINMR0	9
#define PINMR1	10
#define PINML0	5
#define PINML1	6

#define PINSE	11
#define PINSR	12

void setup()  
{
	delay(500);
	pinMode(13, OUTPUT);
	digitalWrite(13, LOW);
	
	pinMode(PINMR0,OUTPUT);
	pinMode(PINMR1,OUTPUT);
	pinMode(PINML0,OUTPUT);
	pinMode(PINML1,OUTPUT);
	
	pinMode(PINSE,OUTPUT);
	pinMode(PINSR,INPUT);
	
	Serial.begin(115200);

	digitalWrite(PINMR0, 0);
	digitalWrite(PINMR1, 1);
	digitalWrite(PINML0, 0);
	digitalWrite(PINML1, 1);

	myinit(0);
	go0 = 1;
}

long x=0, y=0;
char avanzar = 1;
int distancia;

void loop()
{
	uint8_t c[3];
	if(available() >= 3)
	{
		read_byte(&c[0]);
		read_byte(&c[1]);
		read_byte(&c[2]);

		x += (long)((signed char) (((c[0] & 0x03) << 6) | (c[1] & 0x7f)));
		y += (long)(- (signed char) (((c[0] & 0x0C) << 4) | (c[2] & 0x7f)));

		//Serial.print(c[0], HEX);
		//Serial.print(" ");
		//Serial.print(c[1], HEX);
		//Serial.print(" ");
		//Serial.println(c[2], HEX);

		Serial.print(x);
		Serial.print(" ");
		Serial.println(y);
	}
	if(avanzar)
	{
		digitalWrite(PINML0, 0);
		digitalWrite(PINMR0, 0);

		if(y<-10)
			digitalWrite(PINML1, 0);
		else
			digitalWrite(PINML1, 1);

		if(y>10)
			digitalWrite(PINMR1, 0);
		else
			digitalWrite(PINMR1, 1);
	}
	else
	{
			digitalWrite(PINML0, 0);
			digitalWrite(PINML1, 0);
			digitalWrite(PINMR0, 0);
			digitalWrite(PINMR1, 0);
	}


	long tiempo = micros();
	
	digitalWrite(PINSE, 0);
	delayMicroseconds(5);
	digitalWrite(PINSE, 1);
	delayMicroseconds(10);

	tiempo = pulseIn(PINSR, HIGH); /* Función para medir la longitud del pulso entrante. Mide el tiempo que transcurrido entre el envío
				    del pulso ultrasónico y cuando el sensor recibe el rebote, es decir: desde que el pin 12 empieza a recibir el rebote, HIGH, hasta que
				    deja de hacerlo, LOW, la longitud del pulso entrante*/
	int distancia0 = int(0.017*tiempo); /*fórmula para calcular la distancia obteniendo un valor entero*/
	if(distancia0 != distancia)
	{
		distancia = distancia0;
		/*Monitorización en centímetros por el monitor serial*/
		//Serial.print("Distancia ");
		//Serial.print(distancia);
		//Serial.println(" cm");
		if(distancia < 10)
			avanzar = 0;
		else
			avanzar = 1;
	}
}
