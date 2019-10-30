#include "main.h"

static int iSize;
static xSemaphoreHandle xSizeMutex;
static xNode* xMap[MAX_MAP_SIZE];
static xSemaphoreHandle xPutMutex[MAX_TASK_COUNT];
static QueueHandle_t xQueue[MAX_TASK_COUNT];

static void vNumberPut(xTaskParams* task, int hash, long num, long* factors, int* added) {
	const int MAX_REP = 10000;
	int i, contains;

	contains = xListGet(xMap[hash], num) != 0;

	for(i = 0; i < MAX_REP; i++) {
		xMap[hash] = xListPut(xMap[hash], num, factors, added);
		xMap[hash] = xListDelete(xMap[hash], num);
	}

	xMap[hash] = xListPut(xMap[hash], num, factors, added);

	if(!contains) {
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
		
		task->puts++;
	}
}

void vMapInit() {
	int i;

	for(i = 0; i < MAX_TASK_COUNT; i++) {
		xPutMutex[i] = xSemaphoreCreateMutex();
		xQueue[i] = xQueueCreate(MAX_TASK_COUNT, sizeof(xTaskParams*));

		if(!xQueue[i]) {
			printf("Failed to create queue %d\n", i);
			fflush(stdout);
		}
	}

	xSizeMutex = xSemaphoreCreateMutex();
}

int iMapHash(long num) {
	return (num - RANGE_START) % MAX_MAP_SIZE;
}

int iMapSize() {
	return iSize;
}

void vMapRefresh() {
	int i;
	
	for(i = 0; i < MAX_TASK_COUNT; i++) {
		vQueueRefresh(xQueue[i]);
	}
}

int iMapPut(long num, long* factors, xTaskParams* task) {
	int hash = iMapHash(num);
	int added;

	if(iMapSize() >= MAX_MAP_SIZE) {
		return -1;
	}

	if(FLAG_CONCURRENT) {
		int chunk = hash % MAX_TASK_COUNT;

		if(xSemaphoreTake(xPutMutex[chunk], (TickType_t) portMAX_DELAY) == pdTRUE) {
			vNumberPut(task, hash, num, factors, &added);
			xSemaphoreGive(xPutMutex[chunk]);
			vQueueSignal(xQueue[chunk], num);
		}
	}else {
		vNumberPut(task, hash, num, factors, &added);
	}

	if(!added) {
		return -2;
	}

	return 1;
}

long* lMapGet(long num, TickType_t timeout, xTaskParams* task) {
	int hash = iMapHash(num);
	
	xNode* node;

	if(FLAG_CONCURRENT) {
		int chunk = hash % MAX_TASK_COUNT;

		node = xListGet(xMap[hash], num);
		
		if(!node && timeout && task) {
			vQueueWait(xQueue[chunk], num, task, timeout);
			node = xListGet(xMap[hash], num);
		}
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
