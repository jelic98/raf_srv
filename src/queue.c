#include "main.h"

void vQueueWait(QueueHandle_t queue, long num, xTaskParams *task, TickType_t timeout) {
	task->event = num;
	task->deadline = xTaskGetTickCount() + timeout;
	int ok = xQueueSend(queue, (void *) &task, (TickType_t) 0);

	if(!ok) {
		return;
	}

	printf("Getting: %ld Suspending: %d\n", num, task->id);
	fflush(stdout);

	vTaskSuspend(task->handle);
}

void vQueueSignal(QueueHandle_t queue, long num) {
	xTaskParams* first = NULL;
	xTaskParams* current = NULL;

	int ok = xQueueReceive(queue, (void *) &first, (TickType_t) 0);

	if(!ok) {
		return;
	}

	while(current != first) {
		if(!current) {
			current = first;
		}

		if(current->event == num) {
			printf("Putting: %ld Resuming: %d\n", num, current->id);
			fflush(stdout);
			vTaskResume(current->handle);
			return;
		}

		xQueueSend(queue, (void *) &current, (TickType_t) 0);
		xQueueReceive(queue, (void *) &current, (TickType_t) 0);
	}

	xQueueSend(queue, (void *) &current, (TickType_t) 0);
}

void vQueueRefresh(QueueHandle_t queue) {
	xTaskParams* first = NULL;
	xTaskParams* current = NULL;

	int ok = xQueueReceive(queue, (void *) &first, (TickType_t) 0);

	if(!ok) {
		return;
	}

	while(current != first) {
		if(!current) {
			current = first;
		}

		if(xTaskGetTickCount() >= current->deadline) {
			printf("Resuming: %d\n", current->id);
			fflush(stdout);
			vTaskResume(current->handle);
			return;
		}

		xQueueSend(queue, (void *) &current, (TickType_t) 0);
		xQueueReceive(queue, (void *) &current, (TickType_t) 0);
	}

	xQueueSend(queue, (void *) &current, (TickType_t) 0);
}
