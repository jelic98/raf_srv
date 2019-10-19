#include "main.h"

static void vTestStart(int id) {
	printf("TEST %d: START\n", id);
	fflush(stdout);


static void vTestEnd(int id, int ok) {
	if(ok) {
		printf("TEST %d: SUCCESS\n", id);
		fflush(stdout);
	}else {
		printf("TEST %d: FAIL\n", id);
		fflush(stdout);
	}
}

void test_01() {
	vTestStart(1);

	vTasksRun(1);

	vTestEnd(1, 0);
}

void test_02() {
	vTestStart(2);

	vTasksRun(MAX_TASK_COUNT);

	vTestEnd(2, 0);
}
