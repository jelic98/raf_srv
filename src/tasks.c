#include "main.h"

static int iFinishCount;
static xSemaphoreHandle xControlMutex;
static xSemaphoreHandle xControlBarr;
static TaskHandle_t timeoutTask;

static void vTasksInit() {
	int i;

	for(i = 0; i < MAX_TASK_COUNT; i++) {
		xTasks[i].id = i + 1;
		xTasks[i].puts = 0;
		xTasks[i].event = 0;
		xTasks[i].handle = NULL;

		if(i == 0) {
			xTasks[i].start = RANGE_START;
			xTasks[i].end = xTasks[i].start + RANGE_END / MAX_TASK_COUNT - RANGE_START;
		}else {
			xTasks[i].start = xTasks[i - 1].end - RANGE_OVERLAP;
			xTasks[i].end = xTasks[i].start + RANGE_END / MAX_TASK_COUNT + RANGE_OVERLAP;
		}
	}

	xTasks[MAX_TASK_COUNT - 1].end = RANGE_END;

	xControlMutex = xSemaphoreCreateMutex();
	xControlBarr = xSemaphoreCreateCounting(1, 0);

	vMapInit();
}

static void vFactorTask(void* pvParameters) {
	xTaskParams* task = (xTaskParams*) pvParameters;

	if(FLAG_DEBUG) {
		printf("\nTASK %d: Starting in range [%ld - %ld]\n", task->id, task->start, task->end);
		fflush(stdout);
	}

	long i, j, k, num, div;
	int added;

	for(i = task->start; i <= task->end; i++) {
		num = i;
		div = 2;
		j = 0;

		if(FLAG_DEBUG) {
			printf("\nTASK %d: Calculating factors of %ld\n", task->id, num);
			fflush(stdout);
		}

		long factors[MAX_FACT_COUNT] = {0};

		while(num > 1) {
			if(num >= RANGE_START) {
				long* existing = lMapGet(num, TIMEOUT_SLAVE_MILLIS, task);

				if(existing) {
					if(FLAG_DEBUG) {
						printf("TASK %d: Getting factors of %ld from map\n", task->id, num);
						fflush(stdout);
					}

					for(k = 0; existing[k]; k++) {
						factors[j + k] = existing[k];
					}

					break;
				}
			}

			if(num % div) {
				div++;
			}else {
				factors[j++] = div;
				num /= div;
			}
		}

		if(FLAG_DEBUG) {
			printf("TASK %d: Putting factors of %ld to map\n", task->id, i);
			fflush(stdout);

			printf("Factors of %ld:", i);
			fflush(stdout);

			for(j = 0; factors[j]; j++) {
				printf(" %ld", factors[j]);
				fflush(stdout);
			}
		}

		added = iMapPut(i, factors, task);

		if(FLAG_DEBUG) {
			if(added > 0) {
				printf("\nNumber %ld added to map\n", i);
			}else {
				printf("\nNumber %ld not added to map\n", i);

				if(added == -1) {
					printf("Map is full\n");
				}else if (added == -2) {
					printf("Number %ld exists in map\n", i);
				}
			}

			fflush(stdout);
		}
	}

	if(xSemaphoreTake(xControlMutex, (TickType_t) portMAX_DELAY) == pdTRUE) {
		iFinishCount++;

		if(iFinishCount == MAX_TASK_COUNT) {
			xSemaphoreGive(xControlBarr);
		}

		xSemaphoreGive(xControlMutex);
	}

	vTaskDelete(0);
}

static void vTimeoutTask(void* pvParameters) {
	for(;;) {
		vMapRefresh();

		vTaskDelay(TIMEOUT_MASTER_MILLIS / portTICK_PERIOD_MS);
	}

	vTaskDelete(0);
}

static void vControlTask(void* pvParameters) {
	if(xSemaphoreTake(xControlBarr, (TickType_t) portMAX_DELAY) == pdTRUE) {
		vTaskDelete(timeoutTask);

		printf("\nCHECKER: Checking factors in range [%d - %d]\n", RANGE_START, RANGE_END);
		fflush(stdout);

		long i, j, count;

		for(i = RANGE_START; i <= RANGE_END; i++) {
			long* factors = lMapGet(i, 0, NULL);

			if(!factors) {
				printf("\nNumber %ld is not in map\n", i);
				fflush(stdout);
				continue;
			}

			printf("\nFactors of %ld:", i);
			fflush(stdout);

			long product = 1;

			for(j = 0; factors[j]; j++) {
				product *= factors[j];

				printf(" %ld", factors[j]);
				fflush(stdout);
			}

			if(product == i) {
				printf("\nCORRECT: Number=%ld Product=%ld\n", i, product);
				fflush(stdout);
			}else {
				printf("\nINCORRECT: Number=%ld Product=%ld\n", i, product);
				fflush(stdout);
			}
		}

		vMapPrint();

		count = 0;

		for(i = 0; i < MAX_TASK_COUNT; i++) {
			printf("\nTask %ld had %ld puts in range [%ld - %ld]", i + 1, xTasks[i].puts, xTasks[i].start, xTasks[i].end);
			fflush(stdout);
			count += xTasks[i].puts;
		}

		printf("\nTotal put count: %ld\n", count);
		fflush(stdout);

		vMapClear();
	}

	vTaskDelete(0);
}

void vTasksRun() {
	vTasksInit();

	int i;

	for(i = 0; i < MAX_TASK_COUNT; i++) {
		printf("RUNNER: Creating task %d of %d total tasks\n", i + 1, MAX_TASK_COUNT);
		fflush(stdout);

		xTaskCreate(
			vFactorTask,
			(const portCHAR*) "FactorTask",
			configMINIMAL_STACK_SIZE,
			&xTasks[i],
			2,
			&xTasks[i].handle
		);
	}

	xTaskCreate(
		vTimeoutTask,
		(const portCHAR*) "TimeoutTask",
		configMINIMAL_STACK_SIZE,
		NULL,
		3,
		&timeoutTask
	);

	xTaskCreate(
		vControlTask,
		(const portCHAR*) "ControlTask",
		configMINIMAL_STACK_SIZE,
		NULL,
		1,
		NULL
	);

	vTaskStartScheduler();
}
