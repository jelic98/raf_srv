#include "main.h"

static xNode* xMap[MAX_MAP_SIZE];
static xSemaphoreHandle xPutMutex[MAX_TASK_COUNT];
static xSemaphoreHandle xGetSem[RANGE_END - RANGE_START];

void vMapInit() {
	int i;

	for(i = 0; i < MAX_TASK_COUNT; i++) {
		xPutMutex[i] = xSemaphoreCreateMutex();
	}
	
	for(i = 0; i < RANGE_END - RANGE_START; i++) {
		xGetSem[i] = xSemaphoreCreateCounting(MAX_TASK_COUNT, 0);
	}
}

int iMapSize() {
	int i, size = 0;

	for(i = 0; i < MAX_TASK_COUNT; i++) {
		size += xTasks[i].size;
	}

	return size;
}

int iMapHash(long num) {
	return (int) num / MAX_MAP_SIZE;
}

int iMapPut(long num, long* factors) {
	int hash = iMapHash(num);
	int chunk = hash + ((num % (RANGE_END / MAX_TASK_COUNT)) > 0);

	if(iMapSize() >= MAX_MAP_SIZE) {
		return -1;
	}

	int added;

	if(!CONCURRENT || xSemaphoreTake(xPutMutex[chunk], (TickType_t) portMAX_DELAY) == pdTRUE) {
		xMap[hash] = xListPut(xMap[hash], num, factors, &added);
		
		xTasks[chunk].size++;

		if(CONCURRENT) {
			xSemaphoreGive(xPutMutex[chunk]);
		}
		
		if(!added) {
			return -2;
		}

		if(CONCURRENT) {
			xSemaphoreGive(xGetSem[num - RANGE_START]);
		}
	}

	return 0;
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
