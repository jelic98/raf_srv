#include <Arduino_FreeRTOS.h>

#define H_UTIL_IMPLEMENT
#include "util.h"

void setup() {
	vSerialBegin();
	vConsoleSet(vSerialWrite, vSerialRead, iSerialAvailable);
 
	xTaskAddPeriodic(
    "t1",
	  "Printer",
		10,
		50,
	  "A");

  xTaskAddPeriodic(
    "t2",
    "Printer",
    2,
    60,
    "B");
}

void loop() {
	// . . .
}
