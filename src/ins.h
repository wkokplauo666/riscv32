#ifndef INS_H
#define INS_H

#include "../include/emu.h"
#include "cpu.h"

struct ins_r_t {
  uint32_t opcode : 7;
  uint32_t rd : 5;
  uint32_t funct3 : 3;
  uint32_t rs1 : 5;
  uint32_t rs2 : 5;
  uint32_t funct7 : 7;
} __attribute__((packed));

struct ins_i_t {
  uint32_t opcode : 7;
  uint32_t rd : 5;
  uint32_t funct3 : 3;
  uint32_t rs1 : 5;
  uint32_t imm : 12;
} __attribute__((packed));

struct ins_s_t {
  uint32_t opcode : 7;
  uint32_t imml : 5;
  uint32_t funct3 : 3;
  uint32_t rs1 : 5;
  uint32_t rs2 : 5;
  uint32_t immh : 7;
} __attribute__((packed));

struct ins_b_t {
  uint32_t opcode : 7;
  uint32_t imm11 : 1;
  uint32_t imm4_1 : 4;
  uint32_t funct3 : 3;
  uint32_t rs1 : 5;
  uint32_t rs2 : 5;
  uint32_t imm10_5 : 6;
  uint32_t imm12 : 1;
} __attribute__((packed));

struct ins_u_t {
  uint32_t opcode : 7;
  uint32_t rd : 5;
  int32_t immh : 20;
} __attribute__((packed));

struct ins_j_t {
  uint32_t opcode : 7;
  uint32_t rd : 5;
  uint32_t imm19_12 : 8;
  uint32_t imm11 : 1;
  uint32_t imm10_1 : 10;
  uint32_t imm20 : 1;
} __attribute__((packed));

struct ins_general_t {
  uint32_t opcode : 7;
  uint32_t junk : 25;
} __attribute__((packed));

void test();
void invalid_ins(uint32_t ins);

/* RV32I Base instructions */
void ins_arith(uint32_t ins);
void ins_arithi(uint32_t ins);
void ins_load(uint32_t ins);
void ins_store(uint32_t ins);
void ins_jal(uint32_t ins);
void ins_jalr(uint32_t ins);
void ins_lui(uint32_t ins);
void ins_auipc(uint32_t ins);
void ins_branch(uint32_t ins);
void ins_ecall(uint32_t ins);

/* RV32M Multiply extension */
void ins_mulext(uint32_t);

#endif
