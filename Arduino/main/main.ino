#include <Arduino_FreeRTOS.h>

#define H_UTIL_IMPLEMENT
#include "util.h"

static StackType_t xTaskStack[configMINIMAL_STACK_SIZE];
static StaticTask_t xTask;

void vJobPrinter(void* pvParameters) {
  vSerialWrite("%s", (char*) pvParameters);
  vTaskFinish(NULL);
}

void setup() {
	vSerialBegin();
	
	vConsoleSet(vSerialWrite, vSerialRead, iSerialAvailable);

  Serial.println("S");

		xTaskCreatePeriodic(
			vJobPrinter,
			"task1",
			3,
			20,
	    "A",
			xTaskStack,
			&xTask);

      xTaskCreatePeriodic(
      vJobPrinter,
      "task2",
      2,
      10,
      "B",
      xTaskStack,
      &xTask);
}

void loop() {
//  Serial.print("E");
//  Serial.print("hello there");
//	Serial.print("G");
//	Serial.print(",");
//	Serial.print(random(1, 20));
//	Serial.print(",");
//	Serial.println(random(1, 20));
//	delay(1000);

	// . . .
}
