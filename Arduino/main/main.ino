#include <Arduino_FreeRTOS.h>

#define H_SERIAL_IMPLEMENT
#include "serial.h"

static StackType_t xTaskStack[configMINIMAL_STACK_SIZE];
static StaticTask_t xTask;

void vJobPrint(void* pvParameters) {
  char* msg = (char*) pvParameters;

  vSerialWrite(msg);
  
	vTaskFinish(NULL);
}

void setup() {
  Serial.begin(9600);
  
  vConsoleSet(vSerialWrite, vSerialRead);
  
	xTaskCreatePeriodic(
		vJobPrint,
		"task1",
		50,
		100,
    "hello",
		xTaskStack,
		&xTask);
}

void loop() {
  // . . .
}
