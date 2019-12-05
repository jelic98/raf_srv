#include <Arduino_FreeRTOS.h>

#define H_UTIL_IMPLEMENT
#include "util.h"

static StackType_t xTaskStack[configMINIMAL_STACK_SIZE];
static StaticTask_t xTask;

void vJobPrintLetters(void* pvParameters) {
  TaskHandle_t pxTask = (TaskHandle_t) pvParameters;
  int s = xTaskGetTickCount();

  for(;;) {    
    vSerialWrite("%c\n", random('A', 'Z'));
    
    if(xTaskGetTickCount() < s + uxTaskGetCompute(pxTask)) {
      vTaskFinish(NULL);
      s = xTaskGetTickCount();
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
		100,
    "hello",
		xTaskStack,
		&xTask);
}

void loop() {
  // . . .
}
