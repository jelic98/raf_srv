#include "main.h"

static void vTasksInit() {
	int i;

	for(i = 0; i < MAX_TASK_COUNT; i++) {
		xTasks[i].id = i + 1;
		xTasks[i].size = 0;

		if(i == 0) {
			xTasks[i].start = RANGE_START;
			xTasks[i].end = xTasks[i].start + RANGE_END / MAX_TASK_COUNT - RANGE_START;
		}else {
			xTasks[i].start = xTasks[i - 1].end + 1;
			xTasks[i].end = xTasks[i].start + RANGE_END / MAX_TASK_COUNT;
		}
	}

	xTasks[MAX_TASK_COUNT - 1].end = RANGE_END;

	vMapInit();
}

static void vFactorTask(void* pvParameters) {
	xTaskParams* task = (xTaskParams*) pvParameters;

	if(!CONCURRENT || DEBUG) {
		printf("\nTASK %d: Starting in range [%ld - %ld]\n", task->id, task->start, task->end);
		fflush(stdout);
	}

	long i;

	for(i = task->start; i <= task->end; i++) {
		long num = i;
		long div = 2;
		int j = 0;

		long factors[MAX_FACT_COUNT];

		if(!CONCURRENT || DEBUG) {
			printf("\nTASK %d: Calculating factors of %ld\n", task->id, num);
			fflush(stdout);
		}

		while(num > 1) {
			if(num % div) {
				div++;
			}else {
				factors[j++] = div;

				num /= div;

				if(num == 1) {
					break;
				}

				long* existing = lMapGet(num, MAX_TIMEOUT_TICKS);

				if(existing) {
					if(!CONCURRENT || DEBUG) {
						printf("TASK %d: Getting factors of %ld from map\n", task->id, num);
						fflush(stdout);
					}

					int k;

					for(k = 0; existing[k]; k++) {
						factors[k] = existing[k];
					}

					break;
				}
			}
		}

		if(!CONCURRENT || DEBUG) {
			printf("TASK %d: Putting factors of %ld to map\n", task->id, i);
			fflush(stdout);
		}

		iMapPut(i, factors);

		vTaskDelay(MAX_DELAY_MILLIS / portTICK_PERIOD_MS);
	}

	vTaskDelete(0);
}

void vTasksRun() {
	vTasksInit(MAX_TASK_COUNT);

	int i;

	for(i = 0; i < MAX_TASK_COUNT; i++) {
		if(!CONCURRENT || DEBUG) {
			printf("RUNNER: Creating task %d of %d total tasks\n", i + 1, MAX_TASK_COUNT);
			fflush(stdout);
		}

		xTaskCreate(
			vFactorTask,
			(const portCHAR*) "FactorTask",
			configMINIMAL_STACK_SIZE,
			&xTasks[i],
			1,
			NULL
		);
	}

	vTaskStartScheduler();
}
