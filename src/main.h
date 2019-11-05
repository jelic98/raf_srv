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

#define FLAG_DEBUG 0
#define TIMEOUT_SCHED_MILLIS 5

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

void vSchedStart();

#endif
