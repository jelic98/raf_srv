#define H_JOB_IMPLEMENT
#include "job.h"

void vJobPrint(void* pvParameters) {
	portCHAR* pxMessage = (portCHAR*) pvParameters;

	vSerialWrite("%s\n", pxMessage);
}
