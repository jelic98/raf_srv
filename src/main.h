#ifndef H_UTILS_INCLUDE
#define H_UTILS_INCLUDE

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <conio.h>
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "math.h"
#include "time.h"

#define MAX_BUF_SIZE 128
#define MAX_MAP_SIZE 10
#define MAX_TASK_COUNT 10
#define MAX_SEM_COUNT 10
#define MAX_FACT_COUNT 100
#define MAX_TIMEOUT 2000
#define CONCURRENT 1

typedef struct xNode {
	long num;
	long factors[MAX_FACT_COUNT];
	struct xNode* next;
} xNode;

typedef struct xTaskParams {
	int start;
	int end;
	int size;
} xTaskParams;

xTasksParams tasks[MAX_TASK_COUNT];

xNode xListCreate(long num, long* factors);
xNode xListPut(xNode* root, long num, long* factors, int* added);
xNode xListGet(xNode* root, long num);
xNode xListClear(xNode* root);

int vMapInit();
int iMapSize();
int iMapHash(long num);
int iMapPut(long num, long* factors);
long* lMapGet(long num, TickType_t timeout);

void vTasksRun(int count);

void test_01();
void test_02();
void test_03();

#endif
