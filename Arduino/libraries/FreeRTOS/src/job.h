#ifndef H_JOB_INCLUDE
#define H_JOB_INCLUDE

typedef struct JobType_t {
	portCHAR* pcName;
	void (*vJob)(void*);
} JobType_t;

void vJobPrint(void*);

JobType_t pxJobs[] = {
	{"print", &vJobPrint}
};

#endif
