#include <Arduino_FreeRTOS.h>

#define H_UTIL_IMPLEMENT
#include "util.h"

void setup() {
	vSerialBegin();
	vConsoleSet(vSerialWrite, vSerialRead, iSerialAvailable);

	xTaskAddPeriodic(
    "task1",
	  "Printer",
		10,
		50,
	  "A");

  xTaskAddPeriodic(
    "task2",
    "Printer",
    2,
    100,
    "B");
}

void loop() {
	// . . .
}
