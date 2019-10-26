#include "main.h"

static int iSize;
static xSemaphoreHandle xSizeMutex;
static xNode* xMap[MAX_MAP_SIZE];
static xSemaphoreHandle xPutMutex[MAX_TASK_COUNT];
static xSemaphoreHandle xGetSem[MAX_TASK_COUNT];

static void vSizeIncrement() {
	const int MAX_REP = 1000;
	int i;

	if(FLAG_CONCURRENT) {
		if(xSemaphoreTake(xSizeMutex, (TickType_t) portMAX_DELAY) == pdTRUE) {
			for(i = 0; i < MAX_REP; i++) {
				iSize++;
				iSize--;
			}

			iSize++;

			xSemaphoreGive(xSizeMutex);
		}
	}else {
		for(i = 0; i < MAX_REP; i++) {
			iSize++;
			iSize--;
		}

		iSize++;
	}
}

void vMapInit() {
	int i;

	for(i = 0; i < MAX_TASK_COUNT; i++) {
		xPutMutex[i] = xSemaphoreCreateMutex();
		xGetSem[i] = xSemaphoreCreateCounting(MAX_TASK_COUNT, 0);
	}

	xSizeMutex = xSemaphoreCreateMutex();
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
			xMap[hash] = xListDelete(xMap[hash], num);
			xMap[hash] = xListPut(xMap[hash], num, factors, &added);

			xSemaphoreGive(xPutMutex[chunk]);
			xSemaphoreGive(xGetSem[chunk]);
		}
	}else {
		xMap[hash] = xListPut(xMap[hash], num, factors, &added);
		xMap[hash] = xListDelete(xMap[hash], num);
		xMap[hash] = xListPut(xMap[hash], num, factors, &added);
	}

	if(!added) {
		return -2;
	}

	vSizeIncrement();

	xTasks[task].puts++;

	return 1;
}

long* lMapGet(long num, TickType_t timeout) {
	int hash = iMapHash(num);
	
	xNode* node;

	if(FLAG_CONCURRENT) {
		int chunk = hash % MAX_TASK_COUNT;

		TickType_t ticks = xTaskGetTickCount();
		timeout += ticks;

		do {
			if(xSemaphoreTake(xGetSem[chunk], timeout - ticks) == pdTRUE) {
				node = xListGet(xMap[hash], num);

				xSemaphoreGive(xGetSem[chunk]);

				break;
			}

			ticks = xTaskGetTickCount();
		}while(timeout && ticks < timeout);
	}else {
		node = xListGet(xMap[hash], num);
	}

	if(node) {
		return node->factors;
	}

	return NULL;
}

void vMapClear() {
	int i;

	for(i = 0; i < MAX_MAP_SIZE; i++) {
		xMap[i] = xListClear(xMap[i]);
	}

	if(FLAG_CONCURRENT) {
		if(xSemaphoreTake(xSizeMutex, (TickType_t) portMAX_DELAY) == pdTRUE) {
			iSize = 0;
			xSemaphoreGive(xSizeMutex);
		}
	}else {
		iSize = 0;
	}
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
