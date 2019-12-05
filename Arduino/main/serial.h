#ifndef H_SERIAL_INCLUDE

#define SERIAL_BAUD 9600
#define MAX_BUFF_LEN 128

void vSerialBegin();
void vSerialWrite(char*, ...);
void vSerialRead(char*, ...);
 
#ifdef H_SERIAL_IMPLEMENT

void vSerialBegin() {
	Serial.begin(SERIAL_BAUD);
}

void vSerialWrite(char* format, ...) {  
  va_list args;
  va_start(args, format);

  char out[MAX_BUFF_LEN];
  vsprintf(out, format, args);
  Serial.write(out);
  
  va_end(args);
}

void vSerialRead(char* format, ...) {
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
          char* s = (char*) va_arg(args, char*);
          Serial.readBytes(s, MAX_BUFF_LEN);
          break;
      }
    }

    format++;
  }

  Serial.println();

  va_end(args);
}

#endif
#endif
