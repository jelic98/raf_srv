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

static void vTaskInit(TaskType_t* task, void (*fun)(void*), char* name, TickType_t start, TickType_t compute, TickType_t deadline) {
	task->fun = fun;
	strcpy(task->pxName, name);
	task->xStart = start;
	task->xCompute = compute;
	task->xDeadline = deadline;
	task->cState = STATE_READY;
	task->xHandle = NULL;
}

static void vTaskServer(void* pvParameters) {
	TaskType_t* task = (TaskType_t*) pvParameters;
	task->cState = STATE_RUNNING;
	task->fun(task->pvParameters);
	task->cState = STATE_FINISHED;

	vTaskDelete(0);
}

static void vBatchLoad(BatchType_t* batch) {
	char path[MAX_PATH_LEN];

	printf(INPUT_FILE);
	scanf("%s", path);

	FILE* fin = fopen(path, "r");

	if(!fin) {
		printf(ERROR_FILE);
		return;
	}

	int i, iResourceCount, iJobCount, iHeuristicCount, iTaskCount;
	char line[MAX_LINE_LEN];
	char sep[2] = ",";

	fscanf(fin, "%d\n", &iResourceCount);

	// Resources
	for(i = 0; i < iResourceCount && fgets(line, MAX_LINE_LEN, fin); i++) {
		char* name = strtok(line, sep);
		int delay = strtol(strtok(NULL, sep), NULL, 10);
	}

	fscanf(fin, "%d\n", &iJobCount);

	// Jobs
	for(i = 0; i < iJobCount && fgets(line, MAX_LINE_LEN, fin); i++) {
		char* name = strtok(line, sep);
		char* pcResource = strtok(NULL, sep);

		while(pcResource) {
			int resource = strtol(pcResource, NULL, 10);
			pcResource = strtok(NULL, sep);
		}
	}

	fscanf(fin, "%d\n", iHeuristicCount);
	batch->xHeuristicCount = iHeuristicCount;
	batch->pxHeuristics = (HeuristicType_t*) malloc(iHeuristicCount * sizeof(HeuristicType_t));

	// Heuristics
	for(i = 0; i < iHeuristicCount && fgets(line, MAX_LINE_LEN, fin); i++) {
		char* name = strtok(line, sep);
		int order = strtol(strtok(NULL, sep), NULL, 10);
	
		int j = -1;
	
		while(*heuristics[++j].key) {
			if(!strcmp(name, heuristics[j].key)) {
				batch->[i] = heuristics[j];
				break;
			}
		}
	}

	fscanf(fin, "%d\n", &n);
	batch->xTaskCount = iTaskCount;
	batch->pxTasks = (TaskType_t*) malloc(iTaskCount * sizeof(TaskType_t));

	// Tasks
	for(i = 0; i < iTaskCount && fgets(line, MAX_LINE_LEN, fin); i++) {
		char* name = strtok(line, sep);
		int start = strtol(strtok(NULL, sep), NULL, 10);
		int compute = strtol(strtok(NULL, sep), NULL, 10);
		int deadline = strtol(strtok(NULL, sep), NULL, 10);
		int job = strtol(strtok(NULL, sep), NULL, 10);
		char* pcPrecedence = strtok(NULL, sep);

		while(pcPrecedence) {
			int precedence = strtol(pcPrecedence, NULL, 10);
			pcPrecedence = strtok(NULL, sep);
		}
	
		vTaskInit(&(batch->pxTasks[i]), vTaskPrinter, name, start, compute, deadline);
	}

	if(fclose(fin) == EOF) {
		printf(ERROR_FILE);
	}
}

static void vBatchSchedule(BatchType_t* batch) {
	int i, j, p;

	for(i = 0; i < batch->xTaskCount; i++) {
		batch->piSchedule[i] = i;
	}

	for(i = 0; i < batch->xTaskCount - 1; i++) {
		for(j = 0; j < batch->xTaskCount - i - 1; j++) {
			if(batch->cmp[0].fun(batch->pxTasks[j], batch->pxTasks[j + 1]) > 0) {
				p = batch->piSchedule[j];
				batch->piSchedule[j] = batch->piSchedule[j + 1];
				batch->piSchedule[j + 1] = p;
			}
		}
	}
}

static void vSpringScheduler(void* pvParameters) {
	BatchType_t* batch = (BatchType_t*) pvParameters;
	int i;

	for(i = 0; i < batch->xTaskCount; i++) {
		xTaskCreate(vTaskServer,
		            (const portCHAR*) batch->pxTasks[i].name,
		            configMINIMAL_STACK_SIZE,
		            &(batch->pxTasks[i]),
		            PRIORITY_READY,
		            &(batch->pxTasks[i].xHandle));
	}

	fout = fopen(PATH_REPORT, "w");

	for(i = 0; i < batch->xTaskCount; i++) {
		TaskType_t task = batch->pxTasks[batch->piSchedule[i]];
		
		fprintf(fout, "%s,%d\r\n", task.name, xTaskGetTickCount();
		fflush(fout);
		
		vTaskPrioritySet(task.xHandle, PRIORITY_RUN);
	}

	for(;;) {
		int finished = 1;

		for(i = 0; i < batch->xTaskCount; i++) {
			if(batch->xTasks[i].cState != STATE_FINISHED) {
				finished = 0;
				break;
			}
		}

		if(finished) {
			fclose(fout);
			vTaskDelete(NULL);
			break;
		}
		
		vTaskDelay(DELAY_SCHED);
	}
}

void vSchedStart() {
	BatchType_t batch;

	vBatchLoad(&batch);
	vBatchSchedule(&batch);

	xTaskCreate(vSpringScheduler, "", configMINIMAL_STACK_SIZE, &batch, PRIORITY_SCHED, NULL);
	vTaskStartScheduler();
}
