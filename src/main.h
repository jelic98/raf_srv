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

#define STATE_READY 0
#define STATE_RUNNING 1
#define STATE_FINISHED 2
#define INPUT_FILE "Batch file path: "
#define ERROR_FILE "File error occurred\n"

typedef struct JobType_t {
	void (*fun)(void*);
	void* pvParameters;
	TickType_t xArrival;
	TickType_t xDeadline;
	char cState;
	TaskHandle_t xHandle;
} JobType_t;

typedef struct BatchType_t {
	struct JobType_t* pxJobs;
	int* piSchedule;
	BaseType_t xSize;
} BatchType_t;

void vSchedStart();

#endif
