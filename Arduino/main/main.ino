#include <Arduino_FreeRTOS.h>

#define H_UTIL_IMPLEMENT
#include "util.h"

void setup() {
	vSerialBegin();
	vConsoleSet(vSerialWrite, vSerialRead, iSerialAvailable);
/*
TAP,t1,10,50,Printer,a
TAP,t2,2,60,Printer,b
TAP,t3,2,20,Printer,c
*/
}

void loop() {
	// . . .
}
