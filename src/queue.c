void vQueueWait(QueueHandle_t queue, long num, int task, TickType_t timeout) {
	xTasks[task].event = num;
	xTasks[task].deadline = xTaskGetTickCount() + timeout;
	xQueueSend(queue, &task, (TickType_t) 0);
	vTaskSuspend(xTasks[task]);
}

void vQueueSignal(QueueHandle_t queue, long num) {
	int first, current = 0;
	
	xQueueReceive(queue, &first, (TickType_t) 0);

	while(current != first) {
		if(!current) {
			current = first;
		}

		if(xTasks[current].event == num) {
			vTaskResume(xTasks[current].handle);
			break;
		}

		xQueueSend(queue, &current, (TickType_t) 0);
		xQueueReceive(queue, &current, (TickType_t) 0);
	}
}

void vQueueRefresh(QueueHandle_t queue) {
	int first, current = 0;
	
	xQueueReceive(queue, &first, (TickType_t) 0);

	while(current != first) {
		if(!current) {
			current = first;
		}

		if(xTaskGetTickCount() >= xTasks[current].deadline) {
			vTaskResume(xTasks[current].handle);
			break;
		}

		xQueueSend(queue, &current, (TickType_t) 0);
		xQueueReceive(queue, &current, (TickType_t) 0);
	}
}
