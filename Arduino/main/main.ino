/*
Napraviti sistem sa jednim “control” taskom i više “alarm” taskova.

Control task omogućava:
a) Postavljanje novog alarma
b) Štampanje svih alarma
c) Modifikaciju alarma (ime alarma ne može da se modifikuje)
d) Uklanjanje alarma po imenu

Svaki alarm ima sledeća svojstva:
a) String ime, koje se prikazuje pri štampi, i koristi za uklanjanje
b) Vreme kada će da istekne (sekunde, relativno u odnosu na startovanje)
c) Poruka koja se prikazuje kada istekne alarm
d) Svi alarmi su zasebni taskovi koji koriste istu funkciju taska.
e) Alarm task treba da se završi kada mu istekne vreme.
*/

#include <Arduino_FreeRTOS.h>

#define OPT_CREATE 1
#define OPT_PRINT 2
#define OPT_MODIFY 3
#define OPT_DELETE 4
#define DELAY_SEC 1
#define MAX_TASKS 5
#define BUFF_LEN 16
#define SERIAL_BAUD 9600

typedef struct xAlarmTask {
  portCHAR name[BUFF_LEN];
  portCHAR message[BUFF_LEN];
  int timeout;
  int current;
  StackType_t stack[configMINIMAL_STACK_SIZE];
  StaticTask_t static_task;
  TaskHandle_t task_handle;
} xAlarmTask;

static xAlarmTask alarms[MAX_TASKS];
static StackType_t control_stack[configMINIMAL_STACK_SIZE];
static StaticTask_t control_struct;

static void alarm_task(void* pvParameters);
static void control_task(void* pvParameters);

void setup() {
  Serial.begin(SERIAL_BAUD);
  
  xTaskCreateStatic(
    control_task
    , (const portCHAR*) "control"
    , configMINIMAL_STACK_SIZE
    , NULL
    , 1
    , control_stack
    , &control_struct);
}

void loop() {
  // Idle task
}

static void alarm_task(void* pvParameters) {
  xAlarmTask* alarm = (xAlarmTask*) pvParameters;

  while(alarm->current <= alarm->timeout) {
    vTaskDelay(DELAY_SEC * 1000 / portTICK_PERIOD_MS);
    alarm->current += DELAY_SEC;
  }

  writef("%s: %s\n", alarm->name, alarm->message);

  vTaskDelete(0);
}

static void control_task(void* pvParameters) {
  int i;
  int option;
  portCHAR name[BUFF_LEN];
    
  for(;;) {
    writef("%d - Create\n", OPT_CREATE);
    writef("%d - Print\n", OPT_PRINT);
    writef("%d - Modify\n", OPT_MODIFY);
    writef("%d - Delete\n", OPT_DELETE);
    writef("Option?");
    readf("%d", &option);
    writef("Selected option: %d\n", option);

    switch(option) {
      case OPT_CREATE:
        for(i = 0; i < MAX_TASKS; i++) {
          if(!alarms[i].current) {
            writef("Task name?");
            readf("%s", alarms[i].name);
            writef("Selected name: %s\n", alarms[i].name);

            writef("Task message?");
            readf("%s", alarms[i].message);
            writef("Selected message: %s\n", alarms[i].message);

            writef("Task timeout?");
            readf("%d", &alarms[i].timeout);
            writef("Selected timeout: %d\n", alarms[i].timeout);

            alarms[i].current = 0;

            alarms[i].task_handle = xTaskCreateStatic(
              alarm_task
              , alarms[i].name
              , configMINIMAL_STACK_SIZE
              , &alarms[i]
              , 2
              , alarms[i].stack
              , &alarms[i].static_task);
            break;
          }
        }
        break;
      case OPT_PRINT:
        writef("Task name?");
        readf("%s", name);
        writef("Selected name: %s\n", name);

        for(i = 0; i < MAX_TASKS; i++) {
          if(!strcmp(name, alarms[i].name)) {
            writef("Current: %d, Timeout: %d\n", alarms[i].current, alarms[i].timeout);
            break;
          }
        }
        break;
      case OPT_MODIFY:
        writef("Task name?");
        readf("%s", name);
        writef("Selected name: %s\n", name);

        for(i = 0; i < MAX_TASKS; i++) {
          if(!strcmp(name, alarms[i].name)) {
            writef("Task message?");
            readf("%s", alarms[i].message);
            writef("Selected message: %s\n", alarms[i].message);

            writef("Task timeout?");
            readf("%d", &alarms[i].timeout);
            writef("Selected timeout: %d\n", alarms[i].timeout);
            break;
          }
        }
        break;
      case OPT_DELETE:
        writef("Task name?");
        readf("%s", name);
        writef("Selected name: %s\n", name);
        
        for(i = 0; i < MAX_TASKS; i++) {
          if(!strcmp(name, alarms[i].name)) {
            vTaskDelete(alarms[i].task_handle);
            memset(alarms[i].name, 0, sizeof(portCHAR) * BUFF_LEN);
            memset(alarms[i].message, 0, sizeof(portCHAR) * BUFF_LEN);
            alarms[i].timeout = 0;
            alarms[i].current = 0;
            break;
          }
        }
        break;
      default:
        writef("Wrong option.\n");
        break;
    }
  }
}

static void writef(portCHAR* format, ...) {
  va_list args;
  va_start(args, format);

  while(*format) {
    if(*format == '%') {
      switch(*++format) {
        case '%':
          Serial.print('%');
          break;
        case 'd':
          Serial.print(va_arg(args, int));
          break;
        case 'c':
          Serial.print(char(va_arg(args, int)));
          break;
        case 's':
          Serial.print(va_arg(args, portCHAR*));
          break;
      }
    }else {
      Serial.print(*format);
    }

    format++;
  }

  va_end(args);
}

static void readf(portCHAR* format, ...) {
  va_list args;
  va_start(args, format);

  while(!Serial.available());

  while(*format) {
    if(*format == '%') {
      switch(*++format) {
        case 'd':
          *(int*) va_arg(args, int) = Serial.parseInt();
          break;
        case 'c':
          *(int*) va_arg(args, int) = Serial.read();
          break;
        case 's':
          portCHAR* s = (portCHAR*) va_arg(args, portCHAR*);
          Serial.readBytes(s, BUFF_LEN);
          break;
      }
    }

    format++;
  }

  Serial.println();

  va_end(args);
}
