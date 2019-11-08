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
#include <string.h>

#define STATE_READY 0
#define STATE_RUNNING 1
#define STATE_FINISHED 2
#define PATH_REPORT "report.csv"
#define INPUT_FILE "Batch file path: "
#define ERROR_FILE "File error occurred\n"
#define ERROR_SCHEDULE "Schedule is not possible\n"
#define MAX_PATH_LEN 128
#define MAX_LINE_LEN 64
#define MAX_NAME_LEN 16
#define DELAY_SCHED 1
#define PRIORITY_SCHED 3
#define PRIORITY_READY 1
#define PRIORITY_RUN 4
#define HEURISTIC_WEIGHT 0.5f

FILE* pxFout;

typedef struct TaskType_t {
	portCHAR pcName[MAX_NAME_LEN];
	TickType_t xStart;
	TickType_t xCompute;
	TickType_t xDeadline;
	JobType_t* pxJob;
	BaseType_t* pxPrecedence;
	BaseType_t pxPrecedenceCount;
	portCHAR cState;
	TaskHandle_t xHandle;
} TaskType_t;

typedef struct BatchType_t {
	struct TaskType_t* pxTasks;
	struct HeuristicType_t* pxHeuristics;
	BaseType_t xTaskCount;
	BaseType_t xHeuristicCount;
	int* piSchedule;
} BatchType_t;

typedef struct ResourceType_t {
	portCHAR* pcName;
	TickType_t xDelay;
} ResourceType_t;

typedef struct JobType_t {
	portCHAR* pcName;
	void (*fun)(int);
	ResourceType_t* pxResources;
	BaseType_t xResourceCount;
} JobType_t;

typedef struct HeuristicType_t {
	portCHAR* pcName;
	void (*fun)(TaskType_t*, TaskType_t*)*;
	BaseType_t xOrder;

} HeuristicType_t;

void vJobPrinter();
void vJobFactorizer();

JobType_t pxJobs[] = {
	{"Printer", &vJobPrinter},
	{"Factorizer", &vJobFactorizer}
};

int iCompareFCFS(TaskType_t*, TaskType_t*); // First Come First Served
int iCompareSJF(TaskType_t*, TaskType_t*); // Shortest Job First
int iCompareEDF(TaskType_t*, TaskType_t*); // Earliest Deadline First
int iCompareESTF(TaskType_t*, TaskType_t*); // Earliest Start Time First
int iCompareEDFSJF(TaskType_t*, TaskType_t*); // EDF + SJF
int iCompareEDFESTF(TaskType_t*, TaskType_t*); // EDF + ESTF
int iCompareE(TaskType_t*, TaskType_t*); // Eligibility

HeuristicType_t pxHeuristics[] = {
	{"FCFS", &iCompareFCFS},
	{"SJF", &iCompareSJF},
	{"EDF", &iCompareEDF},
	{"ESTF", &iCompareESTF},
	{"EDFSJF", &iCompareEDFSJF},
	{"EDFESTF", &iCompareEDFESTF},
	{"E", &iCompareE}
};

void vSchedStart();

#endif
