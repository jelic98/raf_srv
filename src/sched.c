#include "main.h"

static void vJobCreate(JobType_t* job, void (*fun)(void*), void* pvParameters, TickType_t arrival, TickType_t deadline) {
	job->fun = fun;
	job->pvParameters = pvParameters;
	job->xArrival = arrival;
	job->xDeadline = deadline;
	job->cState = STATE_READY;
	job->xHandle = NULL;
}

static void vJobServer(void* pvParameters) {
	JobType_t* job = (JobType_t*) pvParameters;
	job->cState = STATE_RUNNING;
	job->fun(job->pvParameters);
	job->cState = STATE_FINISHED;

	vTaskDelete(0);
}

static void vJobRun(void* pvParameters) {
	char* msg = (char*) pvParameters;
	int i;

	for(i = 0; i < 20; i++) {
		printf(msg);
		fflush(stdout);
	}

	printf("\n");
	fflush(stdout);
}

static int* piScheduleCreate(JobType_t* jobs, BaseType_t xSize, int* piPrecedenceEdges, int edgesCount) {
	int* flippedResult = (int*) malloc(xSize * sizeof(int));
	int* result = (int*) malloc(xSize * sizeof(int));

	int i, j;

	for(i = 0; i < xSize; i++) {
		flippedResult[i] = -1;
	}

	//nalazimo novi element za result
	for(i = 0; i < xSize; i++) {
		int jobIsNode[xSize]; //0 ako je posao list, 1 ako je cvor, 2 ako je vec rasporedjen
		for(j = 0; j < xSize; j++) {
			jobIsNode[j] = 0;
		}
		for(j = 0; j < xSize; j++) {
			if(flippedResult[j] != -1) {
				jobIsNode[flippedResult[j]] = 2; //upisemo 2 za poslove koji su vec rasporedjeni
			}
		}

		//svaki element koji je cvor belezimo u jobIsNode
		for(j = 0; j < edgesCount; j += 2) {
			int jobId = piPrecedenceEdges[j] - 1;
			if(jobId > -1) {
				jobIsNode[jobId] = 1;
			}
		}

		int maxDeadline = 0;
		int maxDeadlineId = -1;

		for(j = 0; j < xSize; j++) {
			//job is leaf
			if(jobIsNode[j] == 0) {
				if(jobs[j].xDeadline > maxDeadline) {
					maxDeadline = jobs[j].xDeadline;
					maxDeadlineId = j;
				}
			}
		}

		flippedResult[i] = maxDeadlineId;

		//brisanje iz grafa
		for(j = 0; j < edgesCount; j += 2) {
			if(piPrecedenceEdges[j + 1] == maxDeadlineId + 1) {
				piPrecedenceEdges[j] = 0;
				piPrecedenceEdges[j + 1] = 0;
			}
		}
	}

	for(i = 0; i < xSize; i++) {
		result[i] = flippedResult[xSize - i - 1];
	}

	return result;
}

static void vSpringScheduler(void* pvParameters) {
	BatchType_t* batch = (BatchType_t*) pvParameters;
	int i;

	for(i = 0; i < batch->xJobCount; i++) {
		xTaskCreate(vJobServer, "", configMINIMAL_STACK_SIZE, batch->pxJobs + i, 1, &(batch->pxJobs[i].xHandle));
	}

	for(i = 0; i < batch->xJobCount; i++) {
		int currentTask = batch->piSchedule[i];

		vTaskPrioritySet(batch->pxJobs[currentTask].xHandle, 4);
	}

	for(;;) {
		vTaskDelay(1);
	}
}

void vSchedStart() {
	JobType_t job1, job2, job3, job4, job5, job6;
	vJobCreate(&job1, vJobRun, ".", 0, 20);
	vJobCreate(&job2, vJobRun, "-", 0, 50);
	vJobCreate(&job3, vJobRun, "|", 0, 40);
	vJobCreate(&job4, vJobRun, "*", 0, 30);
	vJobCreate(&job5, vJobRun, "?", 0, 50);
	vJobCreate(&job6, vJobRun, "%%", 0, 60);

	BatchType_t batch;
	batch.pxJobs = (JobType_t*) malloc(6 * sizeof(JobType_t));
	batch.xJobCount = 6;
	batch.pxJobs[0] = job1;
	batch.pxJobs[1] = job2;
	batch.pxJobs[2] = job3;
	batch.pxJobs[3] = job4;
	batch.pxJobs[4] = job5;
	batch.pxJobs[5] = job6;

	//neparni brojevi su prethodnici
	//parni brojevi su sledbenici
	//brojevi su logicki indeksi poslova - treba oduzeti 1 za pristup nizu
	int piPrecedenceEdges[] = { 1, 2, 1, 3, 2, 4, 2, 5, 3, 6 };
	int* piSchedule = piScheduleCreate(batch.pxJobs, batch.xJobCount, piPrecedenceEdges, 10);
	int i;

	for(i = 0; i < batch.xJobCount; i++) {
		printf("%d ", piSchedule[i]);
	}
	printf("\n");
	fflush(stdout);

	batch.piSchedule = piSchedule;

	xTaskCreate(vSpringScheduler, "", configMINIMAL_STACK_SIZE, &batch, 3, NULL);
	vTaskStartScheduler();
}
