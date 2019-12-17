#include <Arduino_FreeRTOS.h>

#define H_UTIL_IMPLEMENT
#include "util.h"

void setup() {
	vSerialBegin();
//	vConsoleSet(vSerialWrite, vSerialRead, iSerialAvailable);
//
//		xTaskCreatePeriodic(
//			"Printer",
//			3,
//			20,
//	    "A");
//
//      xTaskCreatePeriodic(
//      "Printer",
//      2,
//      10,
//      "B");
}

void loop() {
//	Serial.print("G");
  Serial.print("C");
	Serial.print(",");
	Serial.print(random(1, 20));
	Serial.print(",");
	Serial.println(random(1, 20));
	delay(1000);

	// . . .
}
