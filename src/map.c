#include "main.h"

static xNode* xMap[MAX_MAP_SIZE];
static xSemaphoreHandle xPutMutex[MAX_SEM_COUNT];
static xSemaphoreHandle xGetSem[MAX_MAP_SIZE];

int vMapInit() {
	int i;

	for(i = 0; i < MAX_TASK_COUNT; i++) {
		tasks[i].size = 0;

		if(i == 0) {
			tasks[i].start = 0;
		}else {
			tasks[i].start = tasks[i - 1].end + 1;
		}

		tasks[i].end = tasks[i].start + floor(MAX_MAP_SIZE / MAX_TASK_COUNT);

		xPutMutex[i] = xSemaphoreCreateMutex();
	}
	
	tasks[MAX_TASK_COUNT - 1] += MAX_MAP_SIZE % MAX_TASK_COUNT;
	
	for(i = 0; i < MAX_MAP_SIZE; i++) {
		xGetSem[i] = xSemaphoreCreateCounting(MAX_TASK_COUNT, 0);
	}
}

int iMapSize() {
	int i, size = 0;

	for(i = 0; i < MAX_TASK_COUNT; i++) {
		size += tasks[i].size;
	}

	return size;
}

int iMapHash(long num) {
	return (int) num / MAX_MAP_SIZE;
}

int iMapPut(long num, long* factors) {
	int hash = iMapHash(num);
	int chunk = hash + ((num % (MAX_MAP_SIZE / MAX_TASK_COUNT)) > 0);

	if(iMapSize() >= MAX_MAP_SIZE) {
		return -1;
	}

	int added;

	if(CONCURRENT && xSemaphoreTake(xPutMutex[chunk], (TickType_t) portMAX_DELAY) == pdTRUE) {
		xMap[hash] = xListPut(xMap[hash], num, factors, &added);
		
		tasks[chunk].size++;

		xSemaphoreGive(xPutMutex[chunk]);
		
		if(!added) {
			return -2;
		}

		xSemaphoreGive(xGetSem[hash]);
	}

	return 0;
}

long* lMapGet(long num, TickType_t timeout) {
	int hash = iMapHash(num);
	
	if(CONCURRENT && xSemaphoreTake(xGetSem[hash], timeout) == pdTRUE) {
		result = xQueue[iReadPtr];

		xNode* node = xListGet(xMap[hash], num);

		if(node) {
			return node->factors;
		}

		xSemaphoreGive(xGetSem[hash]);
	}
	
	return NULL;
}
