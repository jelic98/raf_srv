#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INPUT_FILE "Batch file path: "
#define ERROR_FILE "File error occurred\n"
#define MAX_PATH_LEN 128
#define MAX_LINE_LEN 64

/*
3
disk,10
memorija,20
mutex,30
3
Job 1,0
Job 2,1,2
Job 3
3
Prvi task,10,20,30,0
Drugi task,20,50,100,1,0
Treci task,20,20,30,0
*/

void vBatchLoad() {
	//char path[MAX_PATH_LEN];
	char path[] = "batch.txt";

	//printf(INPUT_FILE);
	//scanf("%s", path);

	FILE* fin = fopen(path, "r");

	if(!fin) {
		printf(ERROR_FILE);
		return;
	}

	int n, i;
	char line[MAX_LINE_LEN];
	char sep[2] = ",";

	fscanf(fin, "%d\n", &n);

	// Resources
	for(i = 0; i < n && fgets(line, MAX_LINE_LEN, fin); i++) {
		char* name = strtok(line, sep);
		int delay = strtol(strtok(NULL, sep), NULL, 10);
	}

	fscanf(fin, "%d\n", &n);

	// Jobs
	for(i = 0; i < n && fgets(line, MAX_LINE_LEN, fin); i++) {
		char* name = strtok(line, sep);
		char* pcResource = strtok(NULL, sep);
		
		while(pcResource) {
			int resource = strtol(pcResource, NULL, 10);
			pcResource = strtok(NULL, sep);
		}
	}

	fscanf(fin, "%d\n", &n);

	// Tasks
	for(i = 0; i < n && fgets(line, MAX_LINE_LEN, fin); i++) {
		char* name = strtok(line, sep);
		int start = strtol(strtok(NULL, sep), NULL, 10);
		int compute = strtol(strtok(NULL, sep), NULL, 10);
		int deadline = strtol(strtok(NULL, sep), NULL, 10);
		int job = strtol(strtok(NULL, sep), NULL, 10);
		char* pcPrecedence = strtok(NULL, sep);
	
		while(pcPrecedence) {
			int precedence = strtol(pcPrecedence, NULL, 10);
			pcPrecedence = strtok(NULL, sep);
		}
	}

	if(fclose(fin) == EOF) {
		printf(ERROR_FILE);
	}
}

int main() {
	vBatchLoad();
	return 0;
}
