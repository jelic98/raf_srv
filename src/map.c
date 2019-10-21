#include "main.h"

static int iSize;
static xNode* xMap[MAX_MAP_SIZE];
static xSemaphoreHandle xSizeMutex;
static xSemaphoreHandle xPutMutex[MAX_TASK_COUNT];
static xSemaphoreHandle xGetSem[RANGE_END - RANGE_START + 1];

void vMapInit() {
	int i;

	xSizeMutex = xSemaphoreCreateMutex();

	for(i = 0; i < MAX_TASK_COUNT; i++) {
		xPutMutex[i] = xSemaphoreCreateMutex();
	}
	
	for(i = 0; i < RANGE_END - RANGE_START + 1; i++) {
		xGetSem[i] = xSemaphoreCreateCounting(MAX_TASK_COUNT, 0);
	}
}

int iMapHash(long num) {
	return (num - RANGE_START) % MAX_MAP_SIZE;
}

int iMapSize() {
	return iSize;
}

int iMapPut(long num, long* factors) {
	int hash = iMapHash(num);
	int chunk = 0;

	if(iMapSize() >= MAX_MAP_SIZE) {
		return -1;
	}

	int added;

	if(!FLAG_CONCURRENT || xSemaphoreTake(xPutMutex[chunk], (TickType_t) portMAX_DELAY) == pdTRUE) {
		xMap[hash] = xListPut(xMap[hash], num, factors, &added);

		if(FLAG_CONCURRENT) {
			xSemaphoreGive(xPutMutex[chunk]);
			xSemaphoreGive(xGetSem[num - RANGE_START]);
		}

		if(!added) {
			return -2;
		}

		if(!FLAG_CONCURRENT || xSemaphoreTake(xSizeMutex, (TickType_t) portMAX_DELAY) == pdTRUE) {
			iSize++;

			if(FLAG_CONCURRENT) {
				xSemaphoreGive(xSizeMutex);
			}
		}
	}

	return 1;
}

long* lMapGet(long num, TickType_t timeout) {
	int hash = iMapHash(num);

	if(!FLAG_CONCURRENT || xSemaphoreTake(xGetSem[num - RANGE_START], timeout) == pdTRUE) {
		xNode* node = xListGet(xMap[hash], num);

		if(FLAG_CONCURRENT) {
			xSemaphoreGive(xGetSem[num - RANGE_START]);
		}

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

	printf("\nMAP (SIZE %d)\n", iMapSize());
	fflush(stdout);

	for(i = 0; i < MAX_MAP_SIZE; i++) {
		printf("#%d ", i);
		fflush(stdout);

		vListPrint(xMap[i]);
	}
}
