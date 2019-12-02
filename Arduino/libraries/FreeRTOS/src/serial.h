#ifndef H_SERIAL_INCLUDE
#define H_SERIAL_INCLUDE

#define SERIAL_BAUD 9600
#define MAX_BUFF_LEN 128

void vSerialBegin();
void vSerialWrite(portCHAR* format, ...);
void vSerialRead(portCHAR* format, ...);

#endif
