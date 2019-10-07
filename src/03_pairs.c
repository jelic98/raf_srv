/*
	Postoji datoteka na disku koja sadrži N parova brojeva. Broj N je zapisan samostalno u prvom redu datoteke.

	Svaki par se zapisuje u zasebnom redu, i brojevi unutar reda su razdvojeni razmakom. Prvi broj je uvek manji od drugog broja u paru.

	Napisati aplikaciju sa dva taska - “èitaè” i “prostask”. Task “èitaè” radi na sledeæi naèin:
	a) Parametar taska je naziv datoteke na disku.
	b) Task èita datoteku i zapisuje proèitane parove u niz ogranièene velièine.
	c) Ako nema mesta u nizu, task treba da se blokira dok je niz pun. Kada ponovo ima mesta u nizu, task treba da se aktivira.

	Task “prostask” radi na sledeæi naèin:
	a) Task se nikada ne završava, veæ radi u beskonaènoj petlji.
	b) Ako postoji par brojeva u nizu koji do sada nije obradio, proèita ga i pronalazi sve proste brojeve u opsegu izmeðu ta dva broja. Kada pronaðe sve proste brojeve u zadatom opsegu, ispisuje ih na ekranu.
	c) Ako su svi parovi brojeva obraðeni, prostask treba da bude blokiran, a ne da se vrti u petlji u kojoj ne radi ništa.
*/

#include "00_utils.h"

#define QUEUE_SIZE 10

static int xMyQueue[QUEUE_SIZE];
static int iWritePtr = 0;
static int iReadPtr = 0;
static int iWriteStart1 = 0;
static int iWriteStart2 = 1000;
static int iWriteStart3 = 2000;

static xSemaphoreHandle xWriteMutex;
static xSemaphoreHandle xQueueEmptySem;
static xSemaphoreHandle xQueueFullSem;

static void vQueuePut(int val) {
	if(xSemaphoreTake(xQueueFullSem, (TickType_t) portMAX_DELAY) == pdTRUE) {
		if(xSemaphoreTake(xWriteMutex, (TickType_t) portMAX_DELAY) == pdTRUE) {
			xMyQueue[iWritePtr] = val;
			iWritePtr = (iWritePtr + 1) % QUEUE_SIZE;

			xSemaphoreGive(xQueueEmptySem);
			xSemaphoreGive(xWriteMutex);
		}
	}
}

static int iQueueTake() {
	int to_return = -1;

	if(xSemaphoreTake(xQueueEmptySem, (TickType_t) portMAX_DELAY) == pdTRUE) {
		to_return = xMyQueue[iReadPtr];
		iReadPtr = (iReadPtr + 1) % QUEUE_SIZE;

		xSemaphoreGive(xQueueFullSem);
	}

	return to_return;
}

static void vWriterTask(void* pvParameters) {
	int startVal = *(int*) pvParameters;
	int i;

	for(i = startVal; i < startVal + 100;) {
		vQueuePut(i);
		printf("Wrote value: %d\n", i);
		fflush(stdout);
		vTaskDelay((TickType_t) 500);
		i++;
	}

	vTaskDelete(0);
}

static void vReaderTask(void* pvParameters) {
	for(;;) {
		int val = iQueueTake();
		printf("Read value: %d\n", val);
		fflush(stdout);
		vTaskDelay(100);
	}
}

void main_03() {
	xWriteMutex = xSemaphoreCreateMutex();
	xQueueEmptySem = xSemaphoreCreateCounting(QUEUE_SIZE, 0);
	xQueueFullSem = xSemaphoreCreateCounting(QUEUE_SIZE, QUEUE_SIZE - 1);

	xTaskCreate(vWriterTask, "", configMINIMAL_STACK_SIZE, &iWriteStart1, 1, NULL);
	xTaskCreate(vWriterTask, "", configMINIMAL_STACK_SIZE, &iWriteStart2, 1, NULL);
	xTaskCreate(vWriterTask, "", configMINIMAL_STACK_SIZE, &iWriteStart3, 1, NULL);

	xTaskCreate(vReaderTask, "", configMINIMAL_STACK_SIZE, NULL, 1, NULL);

	vTaskStartScheduler();
}
