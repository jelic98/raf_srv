#include <Arduino_FreeRTOS.h>

static StackType_t xTaskStack[configMINIMAL_STACK_SIZE];
static StaticTask_t xTask;

void setup() {
	xTaskCreateStatic(
		(const portCHAR*) "print",
		(const portCHAR*) "task1",
		50,
		100,
		xTaskStack,
		&xTask);
}

void loop() {
	// Idle task
}
