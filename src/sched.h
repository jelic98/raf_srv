#ifndef H_SCHED_INCLUDE
#define H_SCHED_INCLUDE

#include "heuristic.h"
#include "job.h"
#include "file.h"

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

typedef struct ResourceType_t {
	portCHAR* pcName;
	TickType_t xDelay;
} ResourceType_t;

typedef struct JobType_t {
	portCHAR* pcName;
	void (*fun)();
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
	portCHAR* pcName;
	BaseType_t (*fun)(TaskType_t*, TaskType_t*);
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

#endif
