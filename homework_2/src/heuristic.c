#include "sched.h"

BaseType_t xCompareFCFS(TaskType_t* t1, TaskType_t* t2) {
	printf("FCFS\t%s=%-5d\t%s=%-5d\n", t1->pcName, t1->xStart, t2->pcName, t2->xStart);
	fflush(stdout);

	return t1->xStart - t2->xStart;
}

BaseType_t xCompareSJF(TaskType_t* t1, TaskType_t* t2) {
	printf("SJF\t%s=%-5d\t%s=%-5d\n", t1->pcName, t1->xCompute, t2->pcName, t2->xCompute);
	fflush(stdout);

	return t1->xCompute - t2->xCompute;
}

BaseType_t xCompareEDF(TaskType_t* t1, TaskType_t* t2) {
	printf("EDF\t%s=%-5d\t%s=%-5d\n", t1->pcName, t1->xDeadline, t2->pcName, t2->xDeadline);
	fflush(stdout);

	return t1->xDeadline - t2->xDeadline;
}

BaseType_t xCompareESTF(TaskType_t* t1, TaskType_t* t2) {
	int i;

	BaseType_t xMaxDelay1 = 0;
	BaseType_t xMaxDelay2 = 0;

	for(i = 0; i < t1->pxJob->xResourceCount; i++) {
		BaseType_t xDelay = t1->pxJob->pxResources[i].xDelay;
		
		if(xDelay > xMaxDelay1) {
			xMaxDelay1 = xDelay;
		}
	}

	for(i = 0; i < t2->pxJob->xResourceCount; i++) {
		BaseType_t xDelay = t2->pxJob->pxResources[i].xDelay;
		
		if(xDelay > xMaxDelay2) {
			xMaxDelay2 = xDelay;
		}
	}

	printf("ESTF\t%s=%-5d\t%s=%-5d\n", t1->pcName, xMaxDelay1, t2->pcName, xMaxDelay2);
	fflush(stdout);

	return xMaxDelay1 - xMaxDelay2;
}

BaseType_t xCompareEDFSJF(TaskType_t* t1, TaskType_t* t2) {
	BaseType_t xResult1 = t1->xDeadline + HEURISTIC_WEIGHT * t1->xCompute;
	BaseType_t xResult2 = t2->xDeadline + HEURISTIC_WEIGHT * t2->xCompute;

	printf("EDFSJF\t%s=%-5d\t%s=%-5d\n", t1->pcName, xResult1, t2->pcName, xResult2);
	fflush(stdout);

	return xResult1 - xResult2;
}

BaseType_t xCompareEDFESTF(TaskType_t* t1, TaskType_t* t2) {
	int i;

	BaseType_t xMaxDelay1 = 0;
	BaseType_t xMaxDelay2 = 0;

	for(i = 0; i < t1->pxJob->xResourceCount; i++) {
		BaseType_t xDelay = t1->pxJob->pxResources[i].xDelay;

		if(xDelay > xMaxDelay1) {
			xMaxDelay1 = xDelay;
		}
	}

	for(i = 0; i < t2->pxJob->xResourceCount; i++) {
		BaseType_t xDelay = t2->pxJob->pxResources[i].xDelay;

		if(xDelay > xMaxDelay2) {
			xMaxDelay2 = xDelay;
		}
	}

	BaseType_t xResult1 = t1->xDeadline + HEURISTIC_WEIGHT * xMaxDelay1;
	BaseType_t xResult2 = t2->xDeadline + HEURISTIC_WEIGHT * xMaxDelay2;

	printf("EDFESTF\t%s=%-5d\t%s=%-5d\n", t1->pcName, xResult1, t2->pcName, xResult2);
	fflush(stdout);

	return xResult1 - xResult2;
}

BaseType_t xCompareE(TaskType_t* t1, TaskType_t* t2) {
	printf("E\t%s=%-5d\t%s=%-5d\n", t1->pcName, t1->xPrecedenceCount, t2->pcName, t2->xPrecedenceCount);
	fflush(stdout);

	return t1->xPrecedenceCount - t2->xPrecedenceCount;
}
