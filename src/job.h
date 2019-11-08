#ifndef H_JOB_INCLUDE
#define H_JOB_INCLUDE

void vJobPrinter();
void vJobFactorizer();

#ifdef H_JOB_IMPLEMENT
	JobType_t pxJobs[] = {
		{"Printer", &vJobPrinter},
		{"Factorizer", &vJobFactorizer}
	};

	HeuristicType_t pxHeuristics[] = {
		{"FCFS", &iCompareFCFS},
		{"SJF", &iCompareSJF},
		{"EDF", &iCompareEDF},
		{"ESTF", &iCompareESTF},
		{"EDFSJF", &iCompareEDFSJF},
		{"EDFESTF", &iCompareEDFESTF},
		{"E", &iCompareE}
	};
#endif
#endif
