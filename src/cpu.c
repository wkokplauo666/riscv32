#include <string.h>
#include <ctype.h>

#include "cpu.h"
#include "ins.h"
#include "memory.h"
#include "io.h"

uint32_t regs[32];
uint32_t pc;

int init_cpu(void) {
  memset(regs, 0, sizeof regs);
  pc = 0x1000;
  regs[2] = 0x5000;

  g_config.flg |= ECT_FLG_CPUOK;

  return 0;
}

void print_reg(void) {
  fprintf(stdlog, "Registers: \n");
  for(int i = 0; i < 32; i += 2) {
    fprintf(stdlog, "x%d   %08x  x%d   %08x\n", i, regs[i], i + 1, regs[i + 1]);
  }
}
 

void dump(const uint8_t *data, uint16_t len, uint16_t offset) {
    const uint16_t bytes_per_line = 16;

    for(uint16_t i = 0; i < len; i += bytes_per_line) {
        fprintf(stdlog, "%08x:  ", offset + i);

        for(uint16_t j = 0; j < bytes_per_line; j++) {
            if(i + j < len) {                fprintf(stdlog, "%02x ", data[i + j + offset]);
            } else {
                fprintf(stdlog, "   ");
            }

            if(j == bytes_per_line / 2 - 1) {
                fprintf(stdlog, " ");
            }
        }

        fprintf(stdlog, " |");
        for(uint16_t j = 0; j < bytes_per_line; j++) {
            if(i + j < len) {
                uint8_t c = data[i + j + offset];
                fprintf(stdlog, "%c", isprint(c) ? c : '.');
            } else {
                fprintf(stdlog, " ");
            }
        }
        fprintf(stdlog, "|\n");
    }
}

uint32_t offset(uint32_t snap) {
    if(pc % snap == 0) {
        return pc;
    } else {
        return pc - (pc % snap);
    }
}


void print_dbg(void) {
  fprintf(stdlog, "\033[H");
  print_reg();
  dump(mem, 0x100, 0xE000);
  fprintf(stdlog, "PC: %04x\n",pc);
  fprintf(stdlog, "Next Instruction: %08x\n", readw(pc));
}


void step(void) {
  regs[0] = 0;
  // printf("\033[?25l");

  uint32_t ins = readw(pc);
  
  uint8_t opc = ((struct ins_general_t *)&ins)->opcode;
  
  switch(opc) {
    case 0b0110011:
      ins_mulext(ins);
      break;
    case 0b0010011:
      ins_arithi(ins);
      break;
    case 0b0000011:
      ins_load(ins);
      break;
    case 0b0100011:
      ins_store(ins);
      break;
    case 0b1101111:
      ins_jal(ins);
      break; 
    case 0b1100111:
      ins_jalr(ins);
      break;
    case 0b0110111:
      ins_lui(ins);
      break;
    case 0b1100011:
      ins_branch(ins);
      break;
    case 0b1110011:
      ins_ecall(ins);
      break;
    default:
      invalid_ins(ins);
      break;
  }
  
  regs[0] = 0;
  update_io();
  //usleep(50000);
}
