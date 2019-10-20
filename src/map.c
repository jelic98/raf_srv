#include "main.h"

static xNode* xMap[MAX_MAP_SIZE];
static xSemaphoreHandle xPutMutex[MAX_TASK_COUNT];
static xSemaphoreHandle xGetSem[RANGE_END - RANGE_START + 1];

void vMapInit() {
	int i;

	for(i = 0; i < MAX_TASK_COUNT; i++) {
		xPutMutex[i] = xSemaphoreCreateMutex();
	}
	
	for(i = 0; i < RANGE_END - RANGE_START + 1; i++) {
		xGetSem[i] = xSemaphoreCreateCounting(MAX_TASK_COUNT, 0);
	}
}

int iMapHash(long num) {
	return num % MAX_MAP_SIZE;
}

int iMapSize() {
	int i, size = 0;

	for(i = 0; i < MAX_TASK_COUNT; i++) {
		size += xTasks[i].size;
	}

	return size;
}

int iMapPut(long num, long* factors, int task) {
	int hash = iMapHash(num);

	if(iMapSize() >= MAX_MAP_SIZE) {
		return -1;
	}

	int added;

	if(!CONCURRENT || xSemaphoreTake(xPutMutex[task], (TickType_t) portMAX_DELAY) == pdTRUE) {
		xMap[hash] = xListPut(xMap[hash], num, factors, &added);

		if(CONCURRENT) {
			xSemaphoreGive(xPutMutex[task]);
		}

		if(!added) {
			return -2;
		}

		if(CONCURRENT) {
			xSemaphoreGive(xGetSem[num - RANGE_START]);
		}
	}

	return 1;
}

long* lMapGet(long num, TickType_t timeout) {
	int hash = iMapHash(num);

	if(!CONCURRENT || xSemaphoreTake(xGetSem[num - RANGE_START], timeout) == pdTRUE) {
		xNode* node = xListGet(xMap[hash], num);

		if(node) {
			return node->factors;
		}

		if(CONCURRENT) {
			xSemaphoreGive(xGetSem[num - RANGE_START]);
		}
	}
	
	return NULL;
}

void vMapClear() {
	int i;

	for(i = 0; i < MAX_MAP_SIZE; i++) {
		xMap[i] = xListClear(xMap[i]);
	}
}
