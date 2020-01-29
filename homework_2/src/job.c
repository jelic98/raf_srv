#include "sched.h"

static char cRandomGet(int min, int max) {
	static int initialized = 0;

	if(!initialized) {
		srand(time(NULL));
		initialized = 1;
	}

    return rand() % (max - min + 1) + min;
}

void vJobPrintLetters(TaskType_t* task) {
	int i;

	for(i = 0; i < JOB_PRINT_COUNT; i++) {
		printf("%s\tLetter\t%c\n", task->pcName, cRandomGet(65, 90));
		fflush(stdout);
	}
}

void vJobPrintNumbers(TaskType_t* task) {
	int i;

	for(i = 0; i < JOB_PRINT_COUNT; i++) {
		printf("%s\tNumber\t%c\n", task->pcName, cRandomGet(48, 57));
		fflush(stdout);
	}
}

void vJobPrintSymbols(TaskType_t* task) {
	int i;

	for(i = 0; i < JOB_PRINT_COUNT; i++) {
		printf("%s\tSymbol\t%c\n", task->pcName, cRandomGet(33, 47));
		fflush(stdout);
	}
}
