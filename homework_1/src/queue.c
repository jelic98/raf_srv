#include "main.h"

void vQueueWait(QueueHandle_t queue, long num, xTaskParams *task, TickType_t timeout) {
	int current = xTaskGetTickCount();
	
	task->event = num;
	task->deadline = current + timeout;
	
	int ok = xQueueSend(queue, (void *) &task, (TickType_t) 0);

	if(!ok) {
		return;
	}

	if(FLAG_DEBUG) {
		printf("Getting: %ld Suspending: %d Time: %d Deadline: %d\n", num, task->id, current, task->deadline);
		fflush(stdout);
	}

	vTaskSuspend(task->handle);
}

void vQueueSignal(QueueHandle_t queue, long num) {
	xTaskParams* first = NULL;
	xTaskParams* current = NULL;

	int ok = xQueueIsQueueEmptyFromISR(queue) == pdFALSE;

	if(!ok) {
		return;
	}

	ok = xQueueReceive(queue, (void *) &first, (TickType_t) 0) == pdTRUE;

	if(!ok) {
		return;
	}

	while(current != first) {
		if(!current) {
			current = first;
		}

		if(current->event == num) {
			if(FLAG_DEBUG) {
				printf("Putting: %ld Resuming: %d Time: %d Deadline: %d\n", num, current->id, xTaskGetTickCount(), current->deadline);
				fflush(stdout);
			}

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

	int ok = xQueueIsQueueEmptyFromISR(queue) == pdFALSE;

	if(!ok) {
		return;
	}

	ok = xQueueReceive(queue, (void *) &first, (TickType_t) 0) == pdTRUE;

	if(!ok) {
		return;
	}

	while(current != first) {
		if(!current) {
			current = first;
		}

		if(xTaskGetTickCount() >= current->deadline) {
			if(FLAG_DEBUG) {
				printf("Resuming: %d Time: %d Deadline: %d\n", current->id, xTaskGetTickCount(), current->deadline);
				fflush(stdout);
			}

			vTaskResume(current->handle);
			return;
		}

		xQueueSend(queue, (void *) &current, (TickType_t) 0);
		xQueueReceive(queue, (void *) &current, (TickType_t) 0);
	}

	xQueueSend(queue, (void *) &current, (TickType_t) 0);
}
