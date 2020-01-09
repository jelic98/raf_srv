/*
DEMO COMMANDS:
SC,5,20
TAP,t1,3,50,Printer,a
TAP,t2,2,60,Printer,b
TAP,t3,1,70,Printer,c
BJP
TAS,3,Sporadic,d
TRP,t1
*/

#include <Arduino_FreeRTOS.h>

#define H_UTIL_IMPLEMENT
#include "util.h"

void setup() {
	vSerialBegin();
	vConsoleSet(vSerialWrite, vSerialRead, iSerialAvailable);
}

void loop() {
	// . . .
}
