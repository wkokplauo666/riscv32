#ifndef EMU_H
#define EMU_H

#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#define STDLOG_FILENO 4

#define ECT_FLG_MEMOK     0x0001
#define ECT_FLG_CPUOK     0x0002

#define ECT_FLG_TERMBUF   0x0010
#define ECT_FLG_TERMTIME  0x0020

#define ECT_FLG_EMUSTEP   0x0100


#define ECT_LTY_LOGNONE   0x0000
#define ECT_LTY_LOGFILE   0x0001
#define ECT_LTY_LOGSTDOUT 0x0002

typedef struct {
  uint16_t flg;
  uint8_t dbglvl;
  uint8_t logtype;
} emu_config_t;

extern FILE *stdlog;
extern emu_config_t g_config;



#endif
