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

#define DELAY_SEC 1
#define MAX_BUF_SIZE 128
#define MAX_MAP_SIZE 10
#define MAX_TASK_COUNT 10
#define MAX_SEM_COUNT 10

typedef struct xNode {
	long value;
	struct xNode* next;
} xNode;

typedef struct xTaskParams {
	int start;
	int end;
	int size;
} xTaskParams;

int iMapPut(long num, long* factors);
long* lMapGet(long num, TickType_t timeout);

void test_01();
void test_02();
void test_03();

#endif
