#define H_FILE_IMPLEMENT
#include "file.h"

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
