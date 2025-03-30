#include "ins.h"
#include "memory.h"

int32_t sext(uint32_t value, uint32_t nbit) {
  if(value & (1U << (nbit - 1))) {
    value |= ~((1U << nbit) - 1);
  }

  return (int32_t) value;
}

void ins_arith(uint32_t ins) {
  struct ins_r_t *inst = (struct ins_r_t *)&ins;
  
  uint32_t f7 = inst->funct7, f3 = inst->funct3, rd = inst->rd, rs1 = inst->rs1, rs2 = inst->rs2;

  if(f7 == 0x00) {
    switch(f3) {
      case 0x00: // ADD
        regs[rd] = regs[rs1] + regs[rs2];
        break;
      case 0x01: // Shift Left Logical
        regs[rd] = regs[rs1] << (regs[rs2] & 0b11111);
        break;
      case 0x02: // Set Less Than
        regs[rd] = ((int32_t) regs[rs1] < (int32_t) regs[rs2]) ? 1 : 0;
        break;
      case 0x03: // Set Less Than (U)
        regs[rd] = (regs[rs1] < regs[rs2]) ? 1 : 0;
        break;
      case 0x04: // XOR
        regs[rd] = regs[rs1] ^ regs[rs2];
        break;
      case 0x05: // Shift Right Logical
        regs[rd] = regs[rs1] >> (regs[rs2] & 0b11111);
        break;
      case 0x06: // OR
        regs[rd] = regs[rs1] | regs[rs2];
        break;
      case 0x7: // AND
        regs[rd] = regs[rs1] & regs[rs2];
        break;
      default:
        invalid_ins(ins);
        break;
    }
  } else if(f7 == 0x20) {
    switch(f3) {
      case 0x00: // SUB
        regs[rd] = regs[rs1] - regs[rs2];
        break;
      case 0x05: // Shift Left Arith*
        regs[rd] = ((int32_t)regs[rs1] >> (regs[rs2] & 0b11111));
        break;
      default:
        invalid_ins(ins);
    }
  } else {
    invalid_ins(ins);
  }

  pc += 4;
}

void ins_arithi(uint32_t ins) {
  struct ins_i_t *inst = (struct ins_i_t *)&ins;
  
  uint32_t f3 = inst->funct3, rd = inst->rd, rs1 = inst->rs1, imm = inst->imm, imml = imm & 0b11111, immh = imm >> 5;
  uint32_t origimm = (uint32_t) imm;

  imm = sext(imm, 12);

  switch(f3) {
    case 0x0: // ADDI
      regs[rd] = (int32_t) imm + regs[rs1];
      break;
    case 0x1:
      if(immh == 0) { // Shift Left Logical Imm
        regs[rd] = regs[rs1] << imml;
      } else {
        invalid_ins(ins);
      }
      break;
    case 0x02: // Set Less Than Imm
      regs[rd] = ((int32_t)regs[rs1] < (int32_t)imm) ? 1 : 0;
      break;
    case 0x03: // Set Less Than Imm Unsigned
      regs[rd] = (regs[rs1] < origimm) ? 1 : 0;
      break;
    case 0x04: // XOR Immidiate
      regs[rd] = regs[rs1] ^ ((int32_t) imm);
      break;
    case 0x05:
      if(immh == 0) { // Shift Right Logical Imm
        regs[rd] = regs[rs1] >> imml;
      } else if(immh == 0x20) { // Shift Right Arith Imm
        regs[rd] = (int32_t) regs[rs1] >> imml;
      } else {
        invalid_ins(ins);
      }
      break;
    case 0x06: // OR Immidiate
      regs[rd] = regs[rs1] | (int32_t) imm;
      break;
    case 0x07: // AND Immidiate
      regs[rd] = regs[rs1] & (int32_t) imm;
      break;
    default:
      invalid_ins(ins);
  }
  
  pc += 4;
}

void ins_load(uint32_t ins) {
  struct ins_i_t *inst = (struct ins_i_t *)&ins;
  uint32_t f3 = inst->funct3, rd = inst->rd, rs1 = inst->rs1, imm = inst->imm;
  
  //printf("old imm l: %04x\n", imm);
  imm = sext(imm, 12);
  //printf("new imm l: %08x\n", imm);


  switch(f3) {
    case 0x0: // Load Byte
      regs[rd] = sext(readb(regs[rs1] + imm), 8);
      break;
    case 0x1: // Load Half-Word
      regs[rd] = sext(readh(regs[rs1] + imm), 16);
      break;
    case 0x2: // Load Word
      regs[rd] = readw(regs[rs1] + imm);
      break;
    case 0x4: // Load Byte Unsigned
      regs[rd] = (uint32_t) readb(regs[rs1] + imm);
      break; 
    case 0x5: // Load Half-Word Unsigned
      regs[rd] = (uint32_t) readh(regs[rs1] + imm);
      break;
    default:
      invalid_ins(ins);
      break;
  }

  pc += 4;
}

void ins_store(uint32_t ins) {
  struct ins_s_t *inst = (struct ins_s_t *)&ins;
  uint32_t f3 = inst->funct3, rs1 = inst->rs1, rs2 = inst->rs2, imml = inst->imml, immh = inst->immh, imm = (immh << 5) | imml;
  
  //printf("old imm s: %04x\n", imm);
  imm = sext(imm, 12);
  //printf("new imm s: %08x\n", imm);

  switch(f3) {
    case 0x0: // Store Byte
      writeb(regs[rs1] + (int32_t)imm, regs[rs2] & 0xff);
      break;
    case 0x1: // Store Half-Word
      writeh(regs[rs1] + (int32_t)imm, regs[rs2] & 0xffff);
      break;
    case 0x02: // Store Word
      writew(regs[rs1] + (int32_t)imm, regs[rs2] & 0xffffffff);
      break;
    default:
      invalid_ins(ins);
      break;
  }

  pc += 4;
}

void ins_jal(uint32_t ins) {
  struct ins_j_t *inst = (struct ins_j_t *)&ins;

  uint32_t imm = (inst->imm20 << 20) | (inst->imm19_12 << 12 ) | (inst->imm11 << 11) | (inst->imm10_1 << 1);
  uint32_t rd = inst->rd;
  imm = sext(imm, 21);

  regs[rd] = pc + 4;
  pc = pc + imm;
}

void ins_jalr(uint32_t ins) {
  struct ins_i_t *inst = (struct ins_i_t *)&ins;

  if(inst->funct3 != 0) {
    invalid_ins(ins);
  }

  regs[inst->rd] = pc + 4;
  pc = (int32_t) regs[inst->rs1] + sext(inst->imm, 12);
}

void ins_lui(uint32_t ins) {
  struct ins_u_t *inst = (struct ins_u_t *)&ins;
  regs[inst->rd] = inst->immh << 12;
  pc += 4;
}

void ins_auipc(uint32_t ins) {
  struct ins_u_t *inst = (struct ins_u_t *)&ins;

  regs[inst->rd] = pc + sext(inst->immh << 12, 20);
  pc += 4;
}

void ins_branch(uint32_t ins) {
  struct ins_b_t *inst = (struct ins_b_t *)&ins;

  uint32_t funct3 = inst->funct3, rs1 = inst->rs1, rs2 = inst->rs2;
  uint32_t imm = (inst->imm12 << 12) | (inst->imm11 << 11) | (inst->imm10_5 << 5) | (inst->imm4_1 << 1);
  
  imm = sext(imm, 13);

  switch(funct3) {
    case 0x00: // BEQ ==
      
      if(regs[rs1] == regs[rs2]) {
        pc += imm;
      } else {
        pc += 4;
      }
      break;
    case 0x01: // BNE !=
      
      if(regs[rs1] != regs[rs2]) {
        pc += imm;
      } else {
        pc += 4;
      }
      break;
    case 0x04: // BLT <
      
      if((int32_t)regs[rs1] < (int32_t)regs[rs2]) {
        pc += imm;
      } else {
        pc += 4;
      }
      break;
    case 0x05: // BGE >=
      if(regs[rs1] >= regs[rs2]) {
        pc += imm;
      } else {
        pc += 4;
      }
      break;
    case 0x06: // BLTU < Unsigned
      if(regs[rs1] < regs[rs2]) {
        pc += imm;
      } else {
        pc += 4;
      }
      break;
    case 0x07: // BGEU >= Unsigned
      if(regs[rs1] >= regs[rs2]) {
        pc += imm;
      } else {
        pc += 4;
      }
      break;
    default:
      invalid_ins(ins);
      break;
  }
}


void ins_mulext(uint32_t ins) {
  struct ins_r_t *inst = (struct ins_r_t *)&ins;
  
  if(inst->funct7 != 0x01) {
    ins_arith(ins);
    return;
  }

  uint32_t f3 = inst->funct3, rd = inst->rd, rs1 = inst->rs1, rs2 = inst->rs2;
  switch(f3) {
    case 0x0: // MUL
      regs[rd] = ((int32_t) regs[rs1] * (int32_t) regs[rs2]) & 0xffffffff;
      break;
    case 0x1: // MULH
      regs[rd] = (uint64_t)((int32_t)regs[rs1] * (int32_t)regs[rs2]) >> 32;
      break;
    case 0x2: // MULSU
      regs[rd] = (uint64_t)((int32_t)regs[rs1] * (uint32_t)regs[rs2]) >> 32;
      break;
    case 0x3: // MULU
      regs[rd] = (uint64_t)((uint32_t)regs[rs1] * (uint32_t)regs[rs2]) >> 32;
      break;
    case 0x4: // DIV
      regs[rd] = (int32_t)regs[rs1] / (int32_t)regs[rs2];
      break;
    case 0x5: // DIVU
      regs[rd] = (uint32_t)regs[rs1] / (uint32_t)regs[rs2];
      break;
    case 0x6: // REM
      regs[rd] = (int32_t)regs[rs1] % (int32_t)regs[rs2];
      break;
    case 0x7: // REMU
      regs[rd] = (uint32_t)regs[rs1] % (uint32_t)regs[rs2];
      break;
    default:
      invalid_ins(ins);
      break;
  }
  pc += 4;
}

void ins_ecall(uint32_t ins) {
  struct ins_i_t *inst = (struct ins_i_t *)&ins;
  if(inst->funct3 != 0) {
    invalid_ins(ins);
  }

  if(inst->imm == 0x0) {
    printf("ecall not implemented\n");
    invalid_ins(ins);
  } else if(inst->imm == 0x1) {
    printf("ebreak not implemented\n");
    invalid_ins(ins);
  } else {
    invalid_ins(ins);
  }
}

void invalid_ins(uint32_t ins) {
  fprintf(stderr, "Invalid instruction %08x at %04x\n", ins, pc);
  exit(1);
}
