#include "sched.h"

BaseType_t xCompareFCFS(TaskType_t* t1, TaskType_t* t2) {
	return t1->xStart - t2->xStart;
}

BaseType_t xCompareSJF(TaskType_t* t1, TaskType_t* t2) {
	return t1->xCompute - t2->xCompute;
}

BaseType_t xCompareEDF(TaskType_t* t1, TaskType_t* t2) {
	return t1->xDeadline - t2->xDeadline;
}

BaseType_t xCompareESTF(TaskType_t* t1, TaskType_t* t2) {
	int i;

	BaseType_t xMinDelay1 = INT_MAX;
	BaseType_t xMinDelay2 = INT_MAX;

	for(i = 0; i < t1->pxJob->xResourceCount; i++) {
		BaseType_t xDelay = t1->pxJob->pxResources[i].xDelay;
		
		if(xDelay < xMinDelay1) {
			xMinDelay1 = xDelay;
		}
	}

	for(i = 0; i < t2->pxJob->xResourceCount; i++) {
		BaseType_t xDelay = t1->pxJob->pxResources[i].xDelay;
		
		if(xDelay < xMinDelay2) {
			xMinDelay2 = xDelay;
		}
	}


	return xMinDelay1 - xMinDelay2;
}

BaseType_t xCompareEDFSJF(TaskType_t* t1, TaskType_t* t2) {
	return (t1->xDeadline + HEURISTIC_WEIGHT * t1->xCompute) - (t2->xDeadline + HEURISTIC_WEIGHT * t2->xCompute);
}

BaseType_t xCompareEDFESTF(TaskType_t* t1, TaskType_t* t2) {
	int i;

	BaseType_t xMinDelay1 = INT_MAX;
	BaseType_t xMinDelay2 = INT_MAX;

	for(i = 0; i < t1->pxJob->xResourceCount; i++) {
		BaseType_t xDelay = t1->pxJob->pxResources[i].xDelay;
		
		if(xDelay < xMinDelay1) {
			xMinDelay1 = xDelay;
		}
	}

	for(i = 0; i < t2->pxJob->xResourceCount; i++) {
		BaseType_t xDelay = t1->pxJob->pxResources[i].xDelay;
		
		if(xDelay < xMinDelay2) {
			xMinDelay2 = xDelay;
		}
	}

	return (t1->xDeadline + HEURISTIC_WEIGHT * xMinDelay1) - (t2->xDeadline + HEURISTIC_WEIGHT * xMinDelay2);
}

BaseType_t xCompareE(TaskType_t* t1, TaskType_t* t2) {
	return t1->xPrecedenceCount - t2->xPrecedenceCount;
}
