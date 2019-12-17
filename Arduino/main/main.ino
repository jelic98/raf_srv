#include <Arduino_FreeRTOS.h>

#define H_UTIL_IMPLEMENT
#include "util.h"

void setup() {
	vSerialBegin();
	vConsoleSet(vSerialWrite, vSerialRead, iSerialAvailable);

		xTaskCreatePeriodic(
      "task1",
			"Printer",
			3,
			20,
	    "A\n");

      xTaskCreatePeriodic(
      "task2",
      "Console",
      2,
      10,
      "B\n");
}

void loop() {
	// . . .
}
