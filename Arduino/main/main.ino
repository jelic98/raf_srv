#include <Arduino_FreeRTOS.h>

#define H_UTIL_IMPLEMENT
#include "util.h"

static StackType_t xTaskStack[configMINIMAL_STACK_SIZE];
static StaticTask_t xTask;

void vJobPrintLetters(void* pvParameters) { 
  static int a = 0;

  TaskHandle_t pxTask = (TaskHandle_t) pvParameters;
  int uxArrival = xTaskGetTickCount();

for(;;) {
  vSerialWrite("%c", 'A' + a++ % 10);
    
  if(xTaskGetTickCount() < uxArrival + uxTaskGetCompute(pxTask)) {
    vSerialWrite("\n");
    vTaskFinish(NULL);
  }
  }
}

void setup() {
  Serial.begin(9600);
  
  vConsoleSet(vSerialWrite, vSerialRead);
  
	xTaskCreatePeriodic(
		vJobPrintLetters,
		"task1",
		50,
		1000,
    "hello",
		xTaskStack,
		&xTask);
}

void loop() {
  // . . .
}
