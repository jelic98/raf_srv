#include "main.h"

static void vFactorTask(void* pvParameters) {
	int i, id = *(int*) pvParameters;

	for(i = tasks[id].start; i <= tasks[id].end; i++) {
		int num = i;
		int div = 2;
		int j = 0;

		long factors[MAX_FACTORS_COUNT];
		
		while(num > 1) {
			if(num % div) {
				div++;
			}else {
				factors[j++] = div;
			
				long* existing = lMapGet(num /= div, MAX_TIMEOUT);

				if(existing) {
					if(!CONCURRENT) {
						printf("Getting factors of %ld from map\n", num);
						fflush(stdout);
					}

					int k, len = sizeof(existing) / sizeof(long);
	
					for(k = 0; k < len; k++) {
						factors[k] = existing[k];
					}
					
					break;
				}
			}
		}
		
		if(!CONCURRENT) {
			printf("Putting factors of %ld to map\n", num);
			fflush(stdout);
		}

		iMapPut(i, factors);
	}

	vTaskDelete(0);
}

void vTasksRun(int count) {
	int i;
	
	for(i = 0; i < count; i++) {
		xTaskCreate(
			vFactorTask,
			(const portCHAR*) "FactorTask",
			configMINIMAL_STACK_SIZE,
			i,
			1,
			NULL
		);
	}

	vTaskStartScheduler();
}
