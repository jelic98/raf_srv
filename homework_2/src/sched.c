#define H_SCHED_IMPLEMENT
#include "sched.h"

static JobType_t pxJobs[] = {
	{"PrintLetters", &vJobPrintLetters},
	{"PrintNumbers", &vJobPrintNumbers},
	{"PrintSymbols", &vJobPrintSymbols}
};

static HeuristicType_t pxHeuristics[] = {
	{"FCFS", &xCompareFCFS},
	{"SJF", &xCompareSJF},
	{"EDF", &xCompareEDF},
	{"ESTF", &xCompareESTF},
	{"EDFSJF", &xCompareEDFSJF},
	{"EDFESTF", &xCompareEDFESTF},
	{"E", &xCompareE}
};

static FILE* pxFout;

static void vReportOpen() {
	pxFout = fopen(PATH_REPORT, "w");
}

static void vReportAdd(TaskType_t* xTask) {
	fprintf(pxFout, "%s,%d\r\n", xTask->pcName, xTaskGetTickCount());
	fflush(pxFout);
}

static void vReportClose() {
	fclose(pxFout);
}

static void vBatchLoad(BatchType_t* pxBatch) {
	//portCHAR path[MAX_PATH_LEN];

	//printf(INPUT_FILE);
	//fflush(stdout);
	//scanf("%s", path);
	portCHAR path[] = "test/batch_10.txt";

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

		while(++j < sizeof(pxJobs) / sizeof(JobType_t)) {
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

		while(++j < sizeof(pxHeuristics) / sizeof(JobType_t)) {
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
			if(pxBatch->pxHeuristics[j].xOrder > pxBatch->pxHeuristics[j + 1].xOrder) {
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
	pxBatch->pxSchedule = (BaseType_t*) malloc(iTaskCount * sizeof(BaseType_t));

	for(i = 0; i < iTaskCount && fgets(pcLine, MAX_LINE_LEN, fin); i++) {
		TaskType_t xTask;
		strcpy(xTask.pcName, strtok(pcLine, pcSep));
		xTask.xStart = strtol(strtok(NULL, pcSep), NULL, 10);
		xTask.xCompute = strtol(strtok(NULL, pcSep), NULL, 10);
		xTask.xDeadline = strtol(strtok(NULL, pcSep), NULL, 10);
		xTask.pxJob = &pxJobs[strtol(strtok(NULL, pcSep), NULL, 10)];
		xTask.cState = STATE_READY;
		xTask.xHandle = NULL;
		xTask.cScheduled = 0;

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

static BaseType_t xBatchCheck(BatchType_t* pxBatch) {
	int i;

	TickType_t xDelay = 0;

	for(i = 0; i < pxBatch->xTaskCount; i++) {
		TaskType_t xTask = pxBatch->pxTasks[pxBatch->pxSchedule[i]];

		if(i && xDelay + xTask.xCompute > xTask.xDeadline) {
			return 0;
		}

		xDelay += xTask.xCompute;
	}

	return 1;
}

static void vBatchTest(BatchType_t* pxBatch) {
	if(!xBatchCheck(pxBatch)) {
		printf(ERROR_SCHEDULE);
		fflush(stdout);
		exit(EXIT_FAILURE);
	}
}

static void vBatchSchedule(BatchType_t* pxBatch) {
	int i, j, p, k = 0;

	for(i = 0; i < pxBatch->xTaskCount; i++) {
		pxBatch->pxSchedule[i] = i;
	}

	do {
		for(i = 0; i < pxBatch->xTaskCount - 1; i++) {
			for(j = 0; j < pxBatch->xTaskCount - i - 1; j++) {
				if(pxBatch->pxHeuristics[k].xCompare(
					&pxBatch->pxTasks[pxBatch->pxSchedule[j]],
					&pxBatch->pxTasks[pxBatch->pxSchedule[j + 1]]) > 0) {
					p = pxBatch->pxSchedule[j];
					pxBatch->pxSchedule[j] = pxBatch->pxSchedule[j + 1];
					pxBatch->pxSchedule[j + 1] = p;
				}
			}
		}
	}while(!xBatchCheck(pxBatch) && ++k < pxBatch->xHeuristicCount);

	printf("Schedule before precedence restriction:\n");

	for(i = 0; i < pxBatch->xTaskCount; i++) {
		printf("%d. %s\n", i + 1, pxBatch->pxTasks[pxBatch->pxSchedule[i]].pcName);
	}

	fflush(stdout);

	BaseType_t* pxSchedule = (BaseType_t*) malloc(pxBatch->xTaskCount * sizeof(BaseType_t));
	BaseType_t xBatchScheduled;

	k = 0;

	do {
		xBatchScheduled = 1;

		for(i = 0; i < pxBatch->xTaskCount; i++) {
			TaskType_t* xTask = &pxBatch->pxTasks[pxBatch->pxSchedule[i]];

			if(xTask->cScheduled) {
				continue;
			}

			xTask->cScheduled = 1;

			for(j = 0; j < xTask->xPrecedenceCount; j++) {
				TaskType_t xParent = pxBatch->pxTasks[xTask->pxPrecedence[j]];

				xTask->cScheduled &= xParent.cScheduled;
			}

			if(xTask->cScheduled) {
				pxSchedule[k++] = pxBatch->pxSchedule[i];
			}

			xBatchScheduled &= xTask->cScheduled;
		}
	}while(!xBatchScheduled);

	for(i = 0; i < pxBatch->xTaskCount; i++) {
		pxBatch->pxSchedule[i] = pxSchedule[i];
	}

	printf("Schedule after precedence restriction:\n");

	for(i = 0; i < pxBatch->xTaskCount; i++) {
		printf("%d. %s\n", i + 1, pxBatch->pxTasks[pxBatch->pxSchedule[i]].pcName);
	}

	fflush(stdout);
}

static void vBatchClear(BatchType_t* pxBatch) {
	int i;

	for(i = 0; i < pxBatch->xTaskCount; i++) {
		TaskType_t xTask = pxBatch->pxTasks[i];

		if(xTask.pxJob->pxResources) {
			free(xTask.pxJob->pxResources);
			xTask.pxJob->pxResources = NULL;
		}
		
		free(xTask.pxPrecedence);
		xTask.pxPrecedence = NULL;
	}

	free(pxBatch->pxTasks);
	free(pxBatch->pxHeuristics);

	pxBatch->pxTasks = NULL;
	pxBatch->pxHeuristics = NULL;
}

static void vSpringServer(void* pvParameters) {
	TaskType_t* xTask = (TaskType_t*) pvParameters;
	xTask->cState = STATE_RUNNING;
	xTask->pxJob->vJobWork(xTask);
	xTask->cState = STATE_FINISHED;

	vTaskDelete(0);
}

static void vSpringScheduler(void* pvParameters) {
	BatchType_t* pxBatch = (BatchType_t*) pvParameters;
	int i;

	for(i = 0; i < pxBatch->xTaskCount; i++) {
		xTaskCreate(vSpringServer,
		            (const portCHAR*) pxBatch->pxTasks[i].pcName,
		            configMINIMAL_STACK_SIZE,
		            &(pxBatch->pxTasks[i]),
		            PRIORITY_READY,
		            &(pxBatch->pxTasks[i].xHandle));
	}

	vReportOpen();

	for(i = 0; i < pxBatch->xTaskCount; i++) {
		TaskType_t xTask = pxBatch->pxTasks[pxBatch->pxSchedule[i]];
		vReportAdd(&xTask);
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
			vReportClose();
			vBatchClear(pxBatch);
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
	vBatchTest(&xBatch);
	return;

	xTaskCreate(vSpringScheduler, (const portCHAR*) "", configMINIMAL_STACK_SIZE, &xBatch, PRIORITY_SCHED, NULL);
	vTaskStartScheduler();
}
