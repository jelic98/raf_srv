#ifndef H_UTILS_INCLUDE
#define H_UTILS_INCLUDE

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <conio.h>
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

#define DELAY_SEC 1
#define MAX_TASKS 10
#define BUFF_SIZE 128

void main_01();
void main_02();
void main_03();

#endif
