#include "main.h"

static int iSize;
static xSemaphoreHandle xSizeMutex;
static xNode* xMap[MAX_MAP_SIZE];
static xSemaphoreHandle xPutMutex[MAX_TASK_COUNT];
static QueueHandle_t xQueue[MAX_TASK_COUNT];

static void vNumberPut(xTaskParams* task, int hash, long num, long* factors, int* added) {
	int contains = xListGet(xMap[hash], num) != 0;

	if(contains) {
		return;
	}

	xMap[hash] = xListPut(xMap[hash], num, factors, added);

	int i;

	for(i = 0; i < MAX_SYNC_REP; i++) {
		iSize++;
		iSize--;
	}

	iSize++;
	task->puts++;
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

int iMapPut(long num, long* factors, xTaskParams* task) {
	int hash = iMapHash(num);
	int added;

	if(iSize >= MAX_MAP_SIZE) {
		return -1;
	}

	if(FLAG_CONCURRENT) {
		int chunk = hash % MAX_TASK_COUNT;

		if(xSemaphoreTake(xPutMutex[chunk], (TickType_t) portMAX_DELAY) == pdTRUE) {
			if(xSemaphoreTake(xSizeMutex, (TickType_t) portMAX_DELAY) == pdTRUE) {
				if(iSize < MAX_MAP_SIZE) {
					vNumberPut(task, hash, num, factors, &added);
					vQueueSignal(xQueue[chunk], num);
				}

				xSemaphoreGive(xSizeMutex);
			}

			xSemaphoreGive(xPutMutex[chunk]);
		}
	}else {
		if(iSize < MAX_MAP_SIZE) {
			vNumberPut(task, hash, num, factors, &added);
		}
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

void vMapRefresh() {
	int i;

	for(i = 0; i < MAX_TASK_COUNT; i++) {
		vQueueRefresh(xQueue[i]);
	}
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

	printf("\nMAP SIZE: %d\n", iSize);
	fflush(stdout);
}
