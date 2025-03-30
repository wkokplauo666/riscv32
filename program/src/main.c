#include <stdint.h>
#include <stdarg.h>

#include "printf.h"

void _start();

__attribute__((naked)) void __entry__() {
  __asm__ __volatile__(
    "li sp, 0x10000\n"
    "call _start\n"
    "j .\n"
  );
}

void __EXIT__() {
  
}

void clrscr() {
  puts("\033c");
}

void fgets(char *buf, size_t bufsiz) {
  size_t i = 0;
  char c;

  if(bufsiz == 0) return;

  while(i < bufsiz - 1) {
    c = getc();

    if(c == '\n') {
      break;
    } else if(c == 0x7f) {
      puts("\b \b");
      i--;
    } else {
      buf[i++] = c;
      putc(c);
    }
  }
  buf[i] = 0;
  puts("\n");
}

int isalpha(char c) {
  return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

uint32_t crc32b(uint8_t *data, size_t len) {
  uint32_t crc = 0xffffffff;
  for(size_t i = 0; i < len; i++) {
    char ch = data[i];
    for(size_t j = 0; j < 8; j++) {
      uint32_t b = (ch ^ crc) & 1;
      crc >>= 1;
      if(b) {
        crc = crc ^ 0x04c11db7;
      }
      ch >>= 1;
    }
  }

  return ~crc;
} 

void _start() { 
  clrscr();
  puts("hi hi welcome this is a testing OS for my emulatddddor ^w^\n");
  
  char buf[100];
  puts("enter a string: ");
  fgets(buf, sizeof buf);

  printf("computet crc32: %x\n", crc32b((uint8_t *)buf, sizeof buf));
}
