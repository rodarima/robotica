#include "ldr/ldr.h"

#include <Arduino.h>
#include "pinout.h"

int ldr0, ldr1;

void ldr_update()
{
	ldr0 = analogRead(PINLDR0);
	ldr1 = analogRead(PINLDR1);
}
