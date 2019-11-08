#define H_SCHED_IMPLEMENT
#include "sched.h"

static void vBatchSchedule(BatchType_t* pxBatch) {
	int i, j, p;

	for(i = 0; i < pxBatch->xTaskCount; i++) {
		pxBatch->pxSchedule[i] = i;
	}

	for(i = 0; i < pxBatch->xTaskCount - 1; i++) {
		for(j = 0; j < pxBatch->xTaskCount - i - 1; j++) {
			if(pxBatch->pxHeuristics[0].xCompare(&pxBatch->pxTasks[j], &pxBatch->pxTasks[j + 1]) > 0) {
				p = pxBatch->pxSchedule[j];
				pxBatch->pxSchedule[j] = pxBatch->pxSchedule[j + 1];
				pxBatch->pxSchedule[j + 1] = p;
			}
		}
	}
}

static void vBatchTest(BatchType_t* pxBatch) {
	int i;

	TickType_t xDelay = 0;

	for(i = 0; i < pxBatch->xTaskCount; i++) {
		TaskType_t xTask = pxBatch->pxTasks[pxBatch->pxSchedule[i]];

		if(i && xDelay + xTask.xCompute > xTask.xDeadline) {
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
		TaskType_t xTask = pxBatch->pxTasks[i];

		if(xTask.xJob.pxResources) {
			free(xTask.xJob.pxResources);
			xTask.xJob.pxResources = NULL;
		}
		
		free(xTask.pxPrecedence);
		xTask.pxPrecedence = NULL;
		
		free(&(pxBatch.pxTasks[i]));
		pxBatch.pxTasks[i] = NULL;
	}

	free(pxBatch->pxTasks);
	free(pxBatch->pxHeuristics);

	pxBatch->pxTasks = NULL;
	pxBatch->pxHeuristics = NULL;
}

static void vSpringServer(void* pvParameters) {
	TaskType_t* xTask = (TaskType_t*) pvParameters;
	xTask->cState = STATE_RUNNING;
	xTask->pxJob->vJobWork();
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
		vReportAdd(xTask);
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
	vBatchTest(pxBatch);

	xTaskCreate(vSpringScheduler, (const portCHAR*) "", configMINIMAL_STACK_SIZE, &xBatch, PRIORITY_SCHED, NULL);
	vTaskStartScheduler();
}
