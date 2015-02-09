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
static volatile uint16_t rx_time;


void store_byte(uint8_t byte);
int read_byte(uint8_t *buf);
void handle_bit();
void handle_interrupt();

void err()
{
	digitalWrite(13, HIGH);
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
	
	attachInterrupt(0, handle_interrupt, rx_mode);
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

	rx_actual_byte = (rx_actual_byte << 1 | bit);

	// First call?
	if(rx_actual_bit == 0)
	{
		//Set the timer to 833.33 us
		//timer2_init(833-300, handle_bit);
		timer2_init(512, handle_bit);
		timer2_start();
	}
	// End of data bits ?
	else if(rx_actual_bit == 6)
	{
		//Then turn off the timer, and write the new byte
		store_byte((rx_inverted_logic ?
				(~rx_actual_byte)
				: rx_actual_byte) & 0x7f);
	
		timer2_init(833*2, handle_last_bit);
		timer2_start();
		Serial.println(diff_time);
	}

	rx_actual_bit++;
	

	sei();
}

void handle_interrupt()
{
	detachInterrupt(0);
	
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

	uint8_t bit = digitalRead(2);

	// Detect logic mode
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


	rx_actual_byte = 0;
	rx_actual_bit = 0;

	timer2_init(416+833-135, handle_bit);
	timer2_start();
}

uint8_t available()
{
	return (rx_head == rx_tail) ? 0 : 1;
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

	pinMode(2, INPUT);
	attachInterrupt(0, handle_interrupt, CHANGE);
}

void setup()  
{
	pinMode(13, OUTPUT);
	digitalWrite(13, LOW);
	Serial.begin(115200);

	myinit(0);
	go0 = 1;
}

void loop()
{
	uint8_t byte;
	if(available())
	{
		if(read_byte(&byte) != 0)
		{
			return;
		}
		//Serial.write(byte);
	}
}
