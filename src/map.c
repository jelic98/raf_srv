#include "main.h"

static int iSize;
static xNode* xMap[MAX_MAP_SIZE];
static xSemaphoreHandle xSizeMutex;
static xSemaphoreHandle xPutMutex[MAX_TASK_COUNT];
static xSemaphoreHandle xGetSem[MAX_TASK_COUNT];

void vMapInit() {
	int i;

	xSizeMutex = xSemaphoreCreateMutex();

	for(i = 0; i < MAX_TASK_COUNT; i++) {
		xPutMutex[i] = xSemaphoreCreateMutex();
		xGetSem[i] = xSemaphoreCreateCounting(MAX_TASK_COUNT, 0);
	}
}

int iMapHash(long num) {
	return (num - RANGE_START) % MAX_MAP_SIZE;
}

int iMapSize() {
	return iSize;
}

int iMapPut(long num, long* factors, int task) {
	int hash = iMapHash(num);
	int added;

	if(iMapSize() >= MAX_MAP_SIZE) {
		return -1;
	}

	if(FLAG_CONCURRENT) {
		int chunk = hash % MAX_TASK_COUNT;

		if(xSemaphoreTake(xPutMutex[chunk], (TickType_t) portMAX_DELAY) == pdTRUE) {
			xMap[hash] = xListPut(xMap[hash], num, factors, &added);

			xSemaphoreGive(xPutMutex[chunk]);
			xSemaphoreGive(xGetSem[chunk]);

			if(!added) {
				return -2;
			}

			if(xSemaphoreTake(xSizeMutex, (TickType_t) portMAX_DELAY) == pdTRUE) {
				iSize++;
				xSemaphoreGive(xSizeMutex);
			}
		}
	}else {
		xMap[hash] = xListPut(xMap[hash], num, factors, &added);

		if(!added) {
			return -2;
		}

		iSize++;
	}

	xTasks[task].puts++;

	return 1;
}

long* lMapGet(long num, TickType_t timeout) {
	int hash = iMapHash(num);
	
	if(FLAG_CONCURRENT) {
		int chunk = hash % MAX_TASK_COUNT;

		TickType_t ticks = xTaskGetTickCount();
		timeout += ticks;

		do {
			if(xSemaphoreTake(xGetSem[chunk], timeout - ticks) == pdTRUE) {
				xNode* node = xListGet(xMap[hash], num);

				xSemaphoreGive(xGetSem[chunk]);

				if(node) {
					return node->factors;
				}
			}

			ticks = xTaskGetTickCount();
		}while(timeout && ticks < timeout);
	}else {
		xNode* node = xListGet(xMap[hash], num);

		if(node) {
			return node->factors;
		}
	}

	return NULL;
}

void vMapClear() {
	int i;

	for(i = 0; i < MAX_MAP_SIZE; i++) {
		xMap[i] = xListClear(xMap[i]);
	}

	iSize = 0;
}

void vMapPrint() {
	int i;

	for(i = 0; i < MAX_MAP_SIZE; i++) {
		printf("\n#%d ", i);
		fflush(stdout);

		vListPrint(xMap[i]);
	}

	printf("\nMAP SIZE: %d\n", iMapSize());
	fflush(stdout);
}
