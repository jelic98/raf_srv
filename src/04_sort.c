/*
Implementirati sortiran niz koji omogućava sledeće operacije:

a) InsertValue(int val)
Upisuje vrednost val u niz, pritom održava sortiranje tako što pomeri sve vrednosti veće od val jedno mesto unapred.
Vraća 0 ako element već postoji, ili ako nema mesta u nizu i vraća 1 ako je ubacivanje uspešno.

b) BinarySearch(int val)
Vrši binarnu pretragu nad nizom da proveri da li se element već nalazi u nizu ili ne.
Vraća -1 ako element nije u nizu, i vraća poziciju elementa ako jeste u nizu.
*/

#include "utils.h"

#define ARRAY_SIZE 10000
#define MAX_INSERTS 2000

static int xSortedArray[ARRAY_SIZE];
static int iElementCount = 0;
static int failed[MAX_TASKS];

static int iBinarySearch(int val) {
	if(!iElementCount) {
		return -1;
	}
	
	int l = 0;
	int r = iElementCount - 1;
	int m, mVal;

	while(l <= r) { 
        m = l + (r - l) / 2; 
  		mVal = xSortedArray[m];

        if(mVal == val) {
            return m;
        }else if(mVal < val) {
            l = m + 1;
        }else {
            r = m - 1;
		}
    } 
  
    return -1 * (l + 1);
}

static int iInsertValue(int val) {
	if(iElementCount >= ARRAY_SIZE) {
		return -1;
	}

	int i, pos = iBinarySearch(val);

	pos = pos < 0 ? -1 * (pos + 1) : pos;

    for(i = iElementCount++; i > pos; i--) {
		xSortedArray[i] = xSortedArray[i - 1]; 	
	}

	xSortedArray[pos] = val;

	return pos;
}

static void vPrintArray() {
	int i;

	for(i = 0; i < iElementCount; i++) {
		if(i > 0) {
			printf(" ");
		}

		printf("%d\n", xSortedArray[i]);
		fflush(stdout);
	}
}

static void vInserterTask(void* pvParameters) {
	srand(time(0));

	int i, num, found, id = *(int*) pvParameters;
	
	for(i = 0; i < MAX_INSERTS; i++) {
		num = rand();
		found = iInsertValue(num);

		if(found < 0) {
			failed[id]++;
		}
	}

	vTaskDelete(0);
}

static void vStatusTask() {
	int sum = failed[0] + failed[1] + failed[2] + failed[3] + failed[4] + failed[5];

	printf("%d %d %d %d %d | %d | %d\n",
			failed[0], failed[1], failed[2], failed[3], failed[4],
			iElementCount, sum);
}

void main_04() {
	int i;

	for(i = 0; i < MAX_TASKS; i++) {
		xTaskCreate(vInserterTask, "", configMINIMAL_STACK_SIZE, &i, 2, NULL);
	}

	xTaskCreate(vStatusTask, "", configMINIMAL_STACK_SIZE, NULL, 1, NULL);

	vTaskStartScheduler();
}

