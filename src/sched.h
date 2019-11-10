#ifndef H_SCHED_INCLUDE
#define H_SCHED_INCLUDE

#include "main.h"

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
#define JOB_PRINT_COUNT 10

typedef struct ResourceType_t {
	portCHAR pcName[MAX_NAME_LEN];
	TickType_t xDelay;
} ResourceType_t;

typedef struct JobType_t {
	portCHAR* pcName;
	void (*vJobWork)();
	ResourceType_t* pxResources;
	BaseType_t xResourceCount;
} JobType_t;

typedef struct TaskType_t {
	portCHAR pcName[MAX_NAME_LEN];
	TickType_t xStart;
	TickType_t xCompute;
	TickType_t xDeadline;
	JobType_t* pxJob;
	BaseType_t* pxPrecedence;
	BaseType_t xPrecedenceCount;
	portCHAR cState;
	TaskHandle_t xHandle;
} TaskType_t;

typedef struct HeuristicType_t {
	portCHAR pcName[MAX_NAME_LEN];
	BaseType_t (*xCompare)(TaskType_t*, TaskType_t*);
	BaseType_t xOrder;

} HeuristicType_t;

typedef struct BatchType_t {
	struct TaskType_t* pxTasks;
	struct HeuristicType_t* pxHeuristics;
	BaseType_t xTaskCount;
	BaseType_t xHeuristicCount;
	BaseType_t* pxSchedule;
} BatchType_t;

void vSchedStart();

void vJobPrintLetters(TaskType_t*);
void vJobPrintNumbers(TaskType_t*);
void vJobPrintSymbols(TaskType_t*);

BaseType_t xCompareFCFS(TaskType_t*, TaskType_t*); // First Come First Served
BaseType_t xCompareSJF(TaskType_t*, TaskType_t*); // Shortest Job First
BaseType_t xCompareEDF(TaskType_t*, TaskType_t*); // Earliest Deadline First
BaseType_t xCompareESTF(TaskType_t*, TaskType_t*); // Earliest Start Time First
BaseType_t xCompareEDFSJF(TaskType_t*, TaskType_t*); // EDF + SJF
BaseType_t xCompareEDFESTF(TaskType_t*, TaskType_t*); // EDF + ESTF
BaseType_t xCompareE(TaskType_t*, TaskType_t*); // Eligibility

#endif
