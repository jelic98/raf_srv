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

#define RANGE_START 2
#define RANGE_END 1000
#define RANGE_OVERLAP 50
#define MAX_MAP_SIZE 1000
#define MAX_TASK_COUNT 15
#define MAX_FACT_COUNT 16
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
int iMapPut(long, long*, int);
long* lMapGet(long, TickType_t);
void vMapClear();
void vMapPrint();

void vTasksRun();
void vTasksCheck();

#endif
