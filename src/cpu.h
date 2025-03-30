#ifndef CPU_H
#define CPU_H

#include "../include/emu.h"

extern uint32_t regs[32];
extern uint32_t pc;

int init_cpu();

void step();
void print_reg();
void print_dbg();

#endif
