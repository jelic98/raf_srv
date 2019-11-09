#include "sched.h"

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
