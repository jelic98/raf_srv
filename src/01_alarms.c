#include "utils.h"

#define OPT_CREATE 1
#define OPT_PRINT 2
#define OPT_MODIFY 3
#define OPT_DELETE 4

typedef struct xAlarmTask {
  char name[BUFF_SIZE];
  char message[BUFF_SIZE];
  int timeout;
  int current;
  xTaskHandle handle;
} xAlarmTask;

static xAlarmTask xAlarms[MAX_TASKS];

static void vAlarmTask(void* pvParameters) {
  xAlarmTask* alarm = (xAlarmTask*) pvParameters;

  while(alarm->current <= alarm->timeout) {
    vTaskDelay(DELAY_SEC * 1000 / portTICK_PERIOD_MS);
    alarm->current += DELAY_SEC;
  }

  printf("%s: %s\n", alarm->name, alarm->message);
  fflush(stdout);

  vTaskDelete(0);
}

static void vControlTask(void* pvParameters) {
  int i;
  int option;
  char name[BUFF_SIZE];

  for(;;) {
    printf("%d - Create\n", OPT_CREATE);
    printf("%d - Print\n", OPT_PRINT);
    printf("%d - Modify\n", OPT_MODIFY);
    printf("%d - Delete\n", OPT_DELETE);
    printf("Option?");
    fflush(stdout);
    scanf("%d", &option);
    printf("Selected option: %d\n", option);
    fflush(stdout);

    switch(option) {
      case OPT_CREATE:
        for(i = 0; i < MAX_TASKS; i++) {
          if(!xAlarms[i].current) {
            printf("Task name?");
            fflush(stdout);
            scanf("%s", xAlarms[i].name);
            printf("Selected name: %s\n", xAlarms[i].name);
            fflush(stdout);

            printf("Task message?");
            fflush(stdout);
            scanf("%s", xAlarms[i].message);
            printf("Selected message: %s\n", xAlarms[i].message);
            fflush(stdout);

            printf("Task timeout?");
            fflush(stdout);
            scanf("%d", &xAlarms[i].timeout);
            printf("Selected timeout: %d\n", xAlarms[i].timeout);
            fflush(stdout);

            xAlarms[i].current = 0;

            xTaskCreate(
              vAlarmTask
              , xAlarms[i].name
              , configMINIMAL_STACK_SIZE
              , &xAlarms[i]
              , 2
              , &xAlarms[i].handle);
            break;
          }
        }
        break;
      case OPT_PRINT:
        printf("Task name?");
        fflush(stdout);
        scanf("%s", name);
        printf("Selected name: %s\n", name);
        fflush(stdout);

        for(i = 0; i < MAX_TASKS; i++) {
          if(!strcmp(name, xAlarms[i].name)) {
            printf("Current: %d, Timeout: %d\n", xAlarms[i].current, xAlarms[i].timeout);
            fflush(stdout);
            break;
          }
        }
        break;
      case OPT_MODIFY:
        printf("Task name?");
    	fflush(stdout);
    	scanf("%s", name);
    	printf("Selected name: %s\n", name);
    	fflush(stdout);

        for(i = 0; i < MAX_TASKS; i++) {
          if(!strcmp(name, xAlarms[i].name)) {
            printf("Task message?");
            fflush(stdout);
            scanf("%s", xAlarms[i].message);
            printf("Selected message: %s\n", xAlarms[i].message);
            fflush(stdout);

            printf("Task timeout?");
            fflush(stdout);
            scanf("%d", &xAlarms[i].timeout);
            printf("Selected timeout: %d\n", xAlarms[i].timeout);
            fflush(stdout);
            break;
          }
        }
        break;
      case OPT_DELETE:
        printf("Task name?");
        fflush(stdout);
        scanf("%s", name);
        printf("Selected name: %s\n", name);
        fflush(stdout);

        for(i = 0; i < MAX_TASKS; i++) {
          if(!strcmp(name, xAlarms[i].name)) {
            vTaskDelete(xAlarms[i].handle);
            memset(xAlarms[i].name, 0, sizeof(char) * BUFF_SIZE);
            memset(xAlarms[i].message, 0, sizeof(char) * BUFF_SIZE);
            xAlarms[i].timeout = 0;
            xAlarms[i].current = 0;
            break;
          }
        }
        break;
      default:
        printf("Wrong option.\n");
        fflush(stdout);
        break;
    }
  }
}

void main_01() {
	xTaskCreate(
	    vControlTask
	    , (const char*) "control"
	    , configMINIMAL_STACK_SIZE
	    , NULL
	    , 1
	    , NULL);

	vTaskStartScheduler();
}
