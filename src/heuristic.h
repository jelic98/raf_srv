#ifndef H_HEURISTIC_INCLUDE
#define H_HEURISTIC_INCLUDE

BaseType_t xCompareFCFS(TaskType_t*, TaskType_t*); // First Come First Served
BaseType_t xCompareSJF(TaskType_t*, TaskType_t*); // Shortest Job First
BaseType_t xCompareEDF(TaskType_t*, TaskType_t*); // Earliest Deadline First
BaseType_t xCompareESTF(TaskType_t*, TaskType_t*); // Earliest Start Time First
BaseType_t xCompareEDFSJF(TaskType_t*, TaskType_t*); // EDF + SJF
BaseType_t xCompareEDFESTF(TaskType_t*, TaskType_t*); // EDF + ESTF
BaseType_t xCompareE(TaskType_t*, TaskType_t*); // Eligibility

#ifdef H_HEURISTIC_IMPLEMENT
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
