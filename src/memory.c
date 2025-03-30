#include <string.h>

#include "memory.h"

uint8_t *mem;

int init_mem() {
  mem = (uint8_t *) malloc(MEMSIZE);
  if(mem == NULL) {
    perror("init_mem() malloc");
    return 1;
  }
  
  memset(mem, 0, MEMSIZE);
  g_config.flg |= ECT_FLG_MEMOK;

  return 0;
}

uint8_t readb(uint16_t addr) {
  return mem[addr % MEMSIZE];
};
void writeb(uint16_t addr, uint8_t data) {
  mem[addr % MEMSIZE] = data;
};

uint16_t readh(uint16_t addr) {
  return *(uint16_t *)&mem[addr % MEMSIZE];
};
void writeh(uint16_t addr, uint16_t data) {
  *(uint16_t *)&mem[addr % MEMSIZE] = data;
};

uint32_t readw(uint16_t addr) {
  return *(uint32_t *)&mem[addr % MEMSIZE];
};
void writew(uint16_t addr, uint32_t data) {
  *(uint32_t *)&mem[addr % MEMSIZE] = data;
};

void memdump(uint16_t addr, uint16_t len) {
  for(uint16_t i = 0; i < len; i++) {
    printf("%02x ", mem[(addr + i) % MEMSIZE]);
  }
  printf("\n");
} 

