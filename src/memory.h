#ifndef MEMORY_H
#define MEMORY_H

#include "../include/emu.h"

#define MEMSIZE 0x20000 // 128Kib

#define BYTE  8
#define HWORD 16
#define WORD  32
#define DWORD 64
#define QWORD 128

extern uint8_t *mem;

int init_mem();

uint8_t readb(uint16_t addr);
void writeb(uint16_t addr, uint8_t data);

uint16_t readh(uint16_t addr);
void writeh(uint16_t addr, uint16_t data);

uint32_t readw(uint16_t addr);
void writew(uint16_t addr, uint32_t data);

uint64_t readd(uint16_t addr);
void writed(uint16_t addr, uint64_t data);


void memdump(uint16_t addr, uint16_t len);

#endif
