/*
Postoji datoteka na disku koja sadrži N parova brojeva.
Broj N je zapisan samostalno u prvom redu datoteke.

Svaki par se zapisuje u zasebnom redu,
	i brojevi unutar reda su razdvojeni razmakom.
Prvi broj je uvek manji od drugog broja u paru.

Napisati aplikaciju sa dva taska - “čitač” i “prostask”.
Task “čitač” radi na sledeći način:
a) Parametar taska je naziv datoteke na disku.
b) Task čita datoteku i zapisuje pročitane parove u niz ograničene veličine.
c) Ako nema mesta u nizu, task treba da se blokira dok je niz pun.
	Kada ponovo ima mesta u nizu, task treba da se aktivira.

Task “prostask” radi na sledeći način:
a) Task se nikada ne završava, već radi u beskonačnoj petlji.
b) Ako postoji par brojeva u nizu koji do sada nije obradio,
	pročita ga i pronalazi sve proste brojeve u opsegu između ta dva broja.
	Kada pronađe sve proste brojeve u zadatom opsegu, ispisuje ih na ekranu.
c) Ako su svi parovi brojeva obrađeni, prostask treba da bude blokiran,
	a ne da se vrti u petlji u kojoj ne radi ništa.
*/

#include "utils.h"

#define QUEUE_SIZE 10
#define MAX_FILES 5

static portCHAR paths[MAX_FILES][BUFF_SIZE] = {
	"Z:\\eclipse-workspace\\raf_srv\\in\\test5.txt",
	"Z:\\eclipse-workspace\\raf_srv\\in\\test4.txt",
	"Z:\\eclipse-workspace\\raf_srv\\in\\test3.txt",
	"Z:\\eclipse-workspace\\raf_srv\\in\\test2.txt",
	"Z:\\eclipse-workspace\\raf_srv\\in\\test1.txt"
};

typedef struct xPair {
	int a;
	int b;
} xPair;

static struct xPair xNullPair;
static xPair xQueue[QUEUE_SIZE];
static int iWritePtr = 0;
static int iReadPtr = 0;

static xSemaphoreHandle xWriteMutex;
static xSemaphoreHandle xReadMutex;
static xSemaphoreHandle xQueueEmptySem;
static xSemaphoreHandle xQueueFullSem;

static void vQueuePut(int a, int b) {
	if(xSemaphoreTake(xQueueFullSem, (TickType_t) portMAX_DELAY) == pdTRUE) {
		if(xSemaphoreTake(xWriteMutex, (TickType_t) portMAX_DELAY) == pdTRUE) {
			xQueue[iWritePtr].a = a;
			xQueue[iWritePtr].b = b;

			iWritePtr = (iWritePtr + 1) % QUEUE_SIZE;

			xSemaphoreGive(xQueueEmptySem);
			xSemaphoreGive(xWriteMutex);
		}
	}
}

static xPair xQueueTake() {
	xPair result = xNullPair;

	if(xSemaphoreTake(xQueueEmptySem, (TickType_t) portMAX_DELAY) == pdTRUE) {
		if(xSemaphoreTake(xReadMutex, (TickType_t) portMAX_DELAY) == pdTRUE) {
			result = xQueue[iReadPtr];
		
			iReadPtr = (iReadPtr + 1) % QUEUE_SIZE;

			xSemaphoreGive(xQueueFullSem);
			xSemaphoreGive(xReadMutex);
		}
	}

	return result;
}

static int iNumberPrime(int n) {
    if(n <= 1) {
        return pdFALSE;
    }

    int i;

    for(i = 2; i <= sqrt(n); i++) {
        if(!(n % i)) {
            return pdFALSE;
        }
    }
    
	return pdTRUE;
}

static void vReadTask(void* pvParameters) {
	portCHAR* path = (portCHAR*) pvParameters;
	FILE* fp = fopen(path, "r");

	printf("Started %s\n", path);
	fflush(stdout);

	if(fp) {
		int n, i, a, b;

		fscanf(fp, "%d", &n);

		for(i = 0; i < n; i++) {
			fscanf(fp, "%d%d", &a, &b);

			printf("Putting (%d, %d) into queue\n", a, b);
			fflush(stdout);

			vQueuePut(a, b);

		    vTaskDelay(DELAY_SEC * 1000 / portTICK_PERIOD_MS);
		}
	
		fclose(fp);
	}
	
	printf("Finished %s\n", path);
	fflush(stdout);

	vTaskDelete(0);
}

static void vPrimeTask(void* pvParameters) {
	int i;

	for(;;) {
		xPair pair = xQueueTake();
		
		printf("Taken (%d, %d) from queue\n", pair.a, pair.b);
		fflush(stdout);

		for(i = pair.a; i <= pair.b; i++) {
			if(iNumberPrime(i)) {
				printf("Prime %d in [%d, %d]\n", i, pair.a, pair.b);
				fflush(stdout);
			}
		}

	    vTaskDelay(DELAY_SEC * 1000 / portTICK_PERIOD_MS);
	}
}

void main_03() {
	xWriteMutex = xSemaphoreCreateMutex();
	xReadMutex = xSemaphoreCreateMutex();
	
	xQueueEmptySem = xSemaphoreCreateCounting(QUEUE_SIZE, 0);
	xQueueFullSem = xSemaphoreCreateCounting(QUEUE_SIZE, QUEUE_SIZE - 1);

	int iCurrFile = MAX_FILES;
	
	while(iCurrFile) {
		xTaskCreate(
			vReadTask,
			(const portCHAR*) "ReadTask",
			configMINIMAL_STACK_SIZE,
			paths[--iCurrFile],
			1,
			NULL
		);
	}

	xTaskCreate(
		vPrimeTask,
		(const portCHAR*) "PrimeTask",
		configMINIMAL_STACK_SIZE,
		NULL,
		1,
		NULL
	);

	vTaskStartScheduler();
}
