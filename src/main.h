#ifndef H_MAIN_INCLUDE
#define H_MAIN_INCLUDE

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <conio.h>
#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"
#include "semphr.h"
#include "math.h"
#include "time.h"

#define RANGE_START 2
#define RANGE_END 1000
#define RANGE_OVERLAP 5
#define MAX_MAP_SIZE 1000
#define MAX_TASK_COUNT 15
#define MAX_FACT_COUNT 16
#define TIMEOUT_MASTER_MILLIS 1
#define TIMEOUT_SLAVE_MILLIS 0
#define FLAG_CONCURRENT 0
#define FLAG_DEBUG 0

typedef struct xNode {
	long num;
	long factors[MAX_FACT_COUNT];
	struct xNode* next;
} xNode;

typedef struct xTaskParams {
	int id;
	long start;
	long end;
	long puts;
	long event;
	TickType_t deadline;
	TaskHandle_t handle;
} xTaskParams;

xTaskParams xTasks[MAX_TASK_COUNT];

xNode* xListCreate(long, long*);
xNode* xListDelete(xNode*, long);
xNode* xListPut(xNode*, long, long*, int*);
xNode* xListGet(xNode*, long);
xNode* xListClear(xNode*);
void vListPrint(xNode*);

void vMapInit();
int iMapHash(long);
int iMapSize();
void vMapRefresh();
int iMapPut(long, long*, xTaskParams*);
long* lMapGet(long, TickType_t, xTaskParams*);
void vMapClear();
void vMapPrint();

void vQueueWait(QueueHandle_t, long, xTaskParams*, TickType_t);
void vQueueSignal(QueueHandle_t, long);
void vQueueRefresh(QueueHandle_t);

void vTasksRun();
void vTasksCheck();

#endif
