#include "main.h"

static int iFinishCount;
static xSemaphoreHandle xControlMutex;
static xSemaphoreHandle xControlBarr;

static int iProbGet() {
	static int initialized = 0;

	if(!initialized) {
		srand(time(0));
		initialized = 1;
	}

    return rand() & 1;
}

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

	xControlMutex = xSemaphoreCreateMutex();
	xControlBarr = xSemaphoreCreateCounting(1, 0);

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

		long factors[MAX_FACT_COUNT] = {0};

		if(!CONCURRENT || DEBUG) {
			printf("\nTASK %d: Calculating factors of %ld\n", task->id, num);
			fflush(stdout);
		}

		if(!CONCURRENT) {
			long repeat = num - iProbGet();

			if(repeat >= RANGE_START) {
				if(repeat != num && DEBUG) {
					printf("TASK %d: Repeating factors of %ld\n", task->id, repeat);
					fflush(stdout);
				}

				i = num = repeat;
			}
		}

		while(num > 1) {
			long* existing = lMapGet(num, MAX_TIMEOUT_TICKS);

			if(existing) {
				if(!CONCURRENT || DEBUG) {
					printf("TASK %d: Getting factors of %ld from map\n", task->id, num);
					fflush(stdout);
				}

				int k;

				for(k = 0; existing[k]; k++) {
					factors[j + k] = existing[k];
				}

				break;
			}

			if(num % div) {
				div++;
			}else {
				factors[j++] = div;

				num /= div;
			}
		}

		if(!CONCURRENT || DEBUG) {
			printf("TASK %d: Putting factors of %ld to map\n", task->id, i);
			fflush(stdout);
		}

		printf("Factors of %ld:", i);
		fflush(stdout);

		for(j = 0; factors[j]; j++) {
			printf(" %ld", factors[j]);
			fflush(stdout);
		}

		int added = iMapPut(i, factors, task->id - 1);

		task->size += added > 0;

		if(!CONCURRENT || DEBUG) {
			if(added > 0) {
				printf("\nNumber %ld added to map\n", i);
				printf("Map size is %d\n", iMapSize());
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

		vTaskDelay(MAX_DELAY_MILLIS / portTICK_PERIOD_MS);
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


static void vControlTask(void* pvParameters) {
	if(xSemaphoreTake(xControlBarr, (TickType_t) portMAX_DELAY) == pdTRUE) {
		if(CONCURRENT && !DEBUG) {
			return;
		}

		long i, j;

		printf("\nCHECKER: Checking factors in range [%d - %d]\n", RANGE_START, RANGE_END);
		fflush(stdout);

		for(i = RANGE_START; i <= RANGE_END; i++) {
			long* factors = lMapGet(i, 0);

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
	}

	vTaskDelete(0);
}

void vTasksRun() {
	vTasksInit();

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
			2,
			NULL
		);
	}

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
