#define H_MAIN_IMPLEMENT
#include "main.h"

void vJobPrinter() {
	int i;

	for(i = 0; i < 10; i++) {
		printf("Printing %d\n", i);
		fflush(stdout);
	}
}

void vJobFactorizer() {
	int i;

	for(i = 0; i < 10; i++) {
		printf("Factorizing %d\n", i);
		fflush(stdout);
	}
}

int iCompareFCFS(TaskType_t* t1, TaskType_t* t2) {
	return t1->xStart - t2->xStart;
}

int iCompareSJF(TaskType_t* t1, TaskType_t* t2) {
	return t1->xCompute - t2->xCompute;
}

int iCompareEDF(TaskType_t* t1, TaskType_t* t2) {
	return t1->xDeadline - t2->xCompute;
}

int iCompareESTF(TaskType_t* t1, TaskType_t* t2) {
	return 0;
}

int iCompareEDFSJF(TaskType_t* t1, TaskType_t* t2) {
	return (t1->xDeadline + HEURISTIC_WEIGHT * t1->xCompute) - (t2->xDeadline + HEURISTIC_WEIGHT * t2->xCompute);
}

int iCompareEDFESTF(TaskType_t* t1, TaskType_t* t2) {
	return 0;
}

int iCompareE(TaskType_t* t1, TaskType_t* t2) {
	return 0;
}

static void vTaskServer(void* pvParameters) {
	TaskType_t* xTask = (TaskType_t*) pvParameters;
	xTask->cState = STATE_RUNNING;
	xTask->pxJob->fun();
	xTask->cState = STATE_FINISHED;

	vTaskDelete(0);
}

static void vBatchLoad(BatchType_t* pxBatch) {
	portCHAR path[MAX_PATH_LEN];

	printf(INPUT_FILE);
	fflush(stdout);
	scanf("%s", path);

	FILE* fin = fopen(path, "r");

	if(!fin) {
		printf(ERROR_FILE);
		fflush(stdout);
		return;
	}

	int i, j, iResourceCount, iJobCount, iHeuristicCount, iTaskCount;
	portCHAR pcLine[MAX_LINE_LEN];
	portCHAR pcSep[2] = ",";

	// Resources
	fscanf(fin, "%d\n", &iResourceCount);
	ResourceType_t* pxResources = (ResourceType_t*) malloc(iResourceCount * sizeof(ResourceType_t));

	for(i = 0; i < iResourceCount && fgets(pcLine, MAX_LINE_LEN, fin); i++) {
		portCHAR* pcName = strtok(pcLine, pcSep);
		TickType_t xDelay = strtol(strtok(NULL, pcSep), NULL, 10);

		strcpy(pxResources[i].pcName, pcName);
		pxResources[i].xDelay = xDelay;
	}

	// Jobs
	fscanf(fin, "%d\n", &iJobCount);

	for(i = 0; i < iJobCount && fgets(pcLine, MAX_LINE_LEN, fin); i++) {
		portCHAR* pcName = strtok(pcLine, pcSep);
		portCHAR* pcResource = strtok(NULL, pcSep);

		int j = -1;

		while(*pxJobs[++j].pcName) {
			if(!strcmp(pcName, pxJobs[j].pcName)) {
				pxJobs[j].pxResources = (ResourceType_t*) malloc(iResourceCount * sizeof(ResourceType_t));
				pxJobs[j].xResourceCount = 0;

				while(pcResource) {
					pxJobs[j].pxResources[pxJobs[j].xResourceCount++] = pxResources[strtol(pcResource, NULL, 10)];

					pcResource = strtok(NULL, pcSep);
				}

				break;
			}
		}
	}

	// Heuristics
	fscanf(fin, "%d\n", &iHeuristicCount);
	pxBatch->xHeuristicCount = 0;
	pxBatch->pxHeuristics = (HeuristicType_t*) malloc(iHeuristicCount * sizeof(HeuristicType_t));

	for(i = 0; i < iHeuristicCount && fgets(pcLine, MAX_LINE_LEN, fin); i++) {
		portCHAR* pcName = strtok(pcLine, pcSep);
		BaseType_t xOrder = strtol(strtok(NULL, pcSep), NULL, 10);

		if(xOrder < 0) {
			continue;
		}

		int j = -1;

		while(*pxHeuristics[++j].pcName) {
			if(!strcmp(pcName, pxHeuristics[j].pcName)) {
				pxBatch->pxHeuristics[pxBatch->xHeuristicCount] = pxHeuristics[j];
				pxBatch->pxHeuristics[pxBatch->xHeuristicCount].xOrder = xOrder;
				pxBatch->xHeuristicCount++;
				break;
			}
		}
	}

	HeuristicType_t pxHeuristic;

	for(i = 0; i < pxBatch->xHeuristicCount - 1; i++) {
		for(j = 0; j < pxBatch->xHeuristicCount - i - 1; j++) {
			if(pxBatch->pxHeuristics[j].xOrder < pxBatch->pxHeuristics[j + 1].xOrder) {
				pxHeuristic = pxBatch->pxHeuristics[j];
				pxBatch->pxHeuristics[j] = pxBatch->pxHeuristics[j + 1];
				pxBatch->pxHeuristics[j + 1] = pxHeuristic;
			}
		}
	}

	// Tasks
	fscanf(fin, "%d\n", &iTaskCount);
	pxBatch->xTaskCount = iTaskCount;
	pxBatch->pxTasks = (TaskType_t*) malloc(iTaskCount * sizeof(TaskType_t));

	for(i = 0; i < iTaskCount && fgets(pcLine, MAX_LINE_LEN, fin); i++) {
		TaskType_t xTask;
		strcpy(xTask.pcName, strtok(pcLine, pcSep));
		xTask.xStart = strtol(strtok(NULL, pcSep), NULL, 10);
		xTask.xCompute = strtol(strtok(NULL, pcSep), NULL, 10);
		xTask.xDeadline = strtol(strtok(NULL, pcSep), NULL, 10);
		xTask.pxJob = &pxJobs[strtol(strtok(NULL, pcSep), NULL, 10)];
		xTask.cState = STATE_READY;
		xTask.xHandle = NULL;

		portCHAR* pcPrecedence = strtok(NULL, pcSep);

		xTask.pxPrecedence = (BaseType_t*) malloc(iTaskCount * sizeof(BaseType_t));
		xTask.xPrecedenceCount = 0;

		while(pcPrecedence) {
			BaseType_t xPrecedence = strtol(pcPrecedence, NULL, 10);

			if(xPrecedence < 0) {
				continue;
			}

			xTask.pxPrecedence[xTask.xPrecedenceCount++] = xPrecedence;

			pcPrecedence = strtok(NULL, pcSep);
		}

		pxBatch->pxTasks[i] = xTask;
	}

	if(fclose(fin) == EOF) {
		printf(ERROR_FILE);
		fflush(stdout);
	}
}

static void vBatchSchedule(BatchType_t* pxBatch) {
	int i, j, p;

	for(i = 0; i < pxBatch->xTaskCount; i++) {
		pxBatch->piSchedule[i] = i;
	}

	for(i = 0; i < pxBatch->xTaskCount - 1; i++) {
		for(j = 0; j < pxBatch->xTaskCount - i - 1; j++) {
			if(pxBatch->pxHeuristics[0].fun(&pxBatch->pxTasks[j], &pxBatch->pxTasks[j + 1]) > 0) {
				p = pxBatch->piSchedule[j];
				pxBatch->piSchedule[j] = pxBatch->piSchedule[j + 1];
				pxBatch->piSchedule[j + 1] = p;
			}
		}
	}

	TickType_t xDelay = 0;

	for(i = 0; i < pxBatch->xTaskCount; i++) {
		TaskType_t xTask = pxBatch->pxTasks[pxBatch->piSchedule[i]];

		if(xDelay + xTask.xCompute > xTask.xDeadline) {
			printf(ERROR_SCHEDULE);
			fflush(stdout);
			exit(EXIT_FAILURE);
		}

		xDelay += xTask.xCompute;
	}
}

static void vBatchClear(BatchType_t* pxBatch) {
	int i;

	for(i = 0; i < pxBatch->xTaskCount; i++) {
		free(pxBatch->pxTasks[i].pxPrecedence);
	}

	// TODO Free allocated memory (resources, precedence)

	free(pxBatch->pxTasks);
	free(pxBatch->pxHeuristics);
}

static void vSpringScheduler(void* pvParameters) {
	BatchType_t* pxBatch = (BatchType_t*) pvParameters;
	int i;

	for(i = 0; i < pxBatch->xTaskCount; i++) {
		xTaskCreate(vTaskServer,
		            (const portCHAR*) pxBatch->pxTasks[i].pcName,
		            configMINIMAL_STACK_SIZE,
		            &(pxBatch->pxTasks[i]),
		            PRIORITY_READY,
		            &(pxBatch->pxTasks[i].xHandle));
	}

	pxFout = fopen(PATH_REPORT, "w");

	for(i = 0; i < pxBatch->xTaskCount; i++) {
		TaskType_t xTask = pxBatch->pxTasks[pxBatch->piSchedule[i]];

		fprintf(pxFout, "%s,%d\r\n", xTask.pcName, xTaskGetTickCount());
		fflush(pxFout);
		
		vTaskPrioritySet(xTask.xHandle, PRIORITY_RUN);
	}

	for(;;) {
		int finished = 1;

		for(i = 0; i < pxBatch->xTaskCount; i++) {
			if(pxBatch->pxTasks[i].cState != STATE_FINISHED) {
				finished = 0;
				break;
			}
		}

		if(finished) {
			vBatchClear(pxBatch);
			fclose(pxFout);
			vTaskDelete(NULL);
			break;
		}

		vTaskDelay(DELAY_SCHED);
	}
}

void vSchedStart() {
	BatchType_t xBatch;

	vBatchLoad(&xBatch);
	vBatchSchedule(&xBatch);

	xTaskCreate(vSpringScheduler, "", configMINIMAL_STACK_SIZE, &xBatch, PRIORITY_SCHED, NULL);
	vTaskStartScheduler();
}
