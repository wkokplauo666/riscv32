#ifndef IO_H
#define IO_H

#include "../include/emu.h"
#include "cpu.h"
#include "memory.h"

/* Console */
#define CONSOLE_DATA  0xE000 // 0xE000-0xE7FF
#define CONSOLE_CMD   0xE800 // 0xE800-0xE80F
#define CLI_CMD     0x00

#define CLI_CMD_FLUSH   0xaa
#define CLI_CMD_IDLE    0x00

/* Keyboard */
#define KEYBORD_DATA  0xE900 // 0xE900-0xE90F
#define KEYBORD_CMD   0xE910 // 0xE910-0xE91F
#define KBD_CMD     0x00

#define KBD_CMD_GETC    0xaa
#define KBD_CMD_IDLE    0x00

void update_io();

#endif
