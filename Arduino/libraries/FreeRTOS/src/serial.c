#define H_SERIAL_IMPLEMENT
#include "serial.h"

void vSerialBegin() {
	Serial.begin(SERIAL_BAUD);
}

void vSerialWrite(portCHAR* format, ...) {
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

void vSerialRead(portCHAR* format, ...) {
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
          Serial.readBytes(s, MAX_BUFF_LEN);
          break;
      }
    }

    format++;
  }

  Serial.println();

  va_end(args);
}
