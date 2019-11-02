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

// 1 = Single task, with synchronization, no timeout
// 2 = Single task, with synchronization, with timeout
// 3 = Multiple tasks, with synchronization, no timeout
// 4 = Multiple tasks, no synchronization, no timeout
#define TEST_ID 1

#if TEST_ID == 1
	#define FLAG_CONCURRENT 1
	#define FLAG_DEBUG 1
	#define RANGE_END 100
	#define MAX_MAP_SIZE 50
	#define MAX_TASK_COUNT 1
	#define TIMEOUT_SLAVE_MILLIS 0

#elif TEST_ID == 2
	#define FLAG_CONCURRENT 1
	#define FLAG_DEBUG 1
	#define RANGE_END 100
	#define MAX_MAP_SIZE 50
	#define MAX_TASK_COUNT 1
	#define TIMEOUT_SLAVE_MILLIS 10

#elif TEST_ID == 3
	#define FLAG_CONCURRENT 1
	#define FLAG_DEBUG 0
	#define RANGE_END 10000
	#define MAX_MAP_SIZE 5000
	#define MAX_TASK_COUNT 50
	#define TIMEOUT_SLAVE_MILLIS 0

#elif TEST_ID == 4
	#define FLAG_CONCURRENT 0
	#define FLAG_DEBUG 0
	#define RANGE_END 10000
	#define MAX_MAP_SIZE 5000
	#define MAX_TASK_COUNT 50
	#define TIMEOUT_SLAVE_MILLIS 0
#endif

#define RANGE_START 2
#define RANGE_OVERLAP 150
#define MAX_FACT_COUNT 16
#define MAX_SYNC_REP 10000
#define TIMEOUT_MASTER_MILLIS 5

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
xNode* xListPut(xNode*, long, long*, int*);
xNode* xListGet(xNode*, long);
xNode* xListClear(xNode*);
void vListPrint(xNode*);

void vMapInit();
int iMapHash(long);
int iMapPut(long, long*, xTaskParams*);
long* lMapGet(long, TickType_t, xTaskParams*);
void vMapRefresh();
void vMapClear();
void vMapPrint();

void vQueueWait(QueueHandle_t, long, xTaskParams*, TickType_t);
void vQueueSignal(QueueHandle_t, long);
void vQueueRefresh(QueueHandle_t);

void vTasksRun();
void vTasksCheck();

#endif
