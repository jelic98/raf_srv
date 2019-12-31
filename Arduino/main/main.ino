#include <Arduino_FreeRTOS.h>

#define H_UTIL_IMPLEMENT
#include "util.h"

void setup() {
	vSerialBegin();
	vConsoleSet(vSerialWrite, vSerialRead, iSerialAvailable);
/*

SC,5,30

TAP,t1,3,50,Printer,a
TAP,t2,2,60,Printer,b
TAP,t3,2,20,Printer,c

BJP

TAS,3,Sporadic,d

*/
}

void loop() {
	// . . .
}
