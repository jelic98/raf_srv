#ifndef H_MAIN_INCLUDE
#define H_MAIN_INCLUDE

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <conio.h>
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "math.h"
#include "time.h"

#define RANGE_START 10
#define RANGE_END 20
#define MAX_MAP_SIZE 50
#define MAX_TASK_COUNT 1
#define MAX_FACT_COUNT 16
#define MAX_TIMEOUT_TICKS 10000
#define MAX_DELAY_MILLIS 100
#define CONCURRENT 0
#define DEBUG 1
#define TEST_ID 1

typedef struct xNode {
	long num;
	long factors[MAX_FACT_COUNT];
	struct xNode* next;
} xNode;

typedef struct xTaskParams {
	int id;
	long size;
	long start;
	long end;
} xTaskParams;

xTaskParams xTasks[MAX_TASK_COUNT];

xNode* xListCreate(long, long*);
xNode* xListPut(xNode*, long, long*, int*);
xNode* xListGet(xNode*, long);
xNode* xListClear(xNode*);

void vMapInit();
int iMapSize();
int iMapHash(long);
int iMapPut(long, long*);
long* lMapGet(long, TickType_t);

void vTasksRun();
void vTasksCheck();

#endif
