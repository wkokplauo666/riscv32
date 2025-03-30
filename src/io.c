#include <stdio.h>

#include "io.h"

void update_io() {
  if(readb(CONSOLE_CMD + CLI_CMD) == CLI_CMD_FLUSH) {
    fputs((const char *)&mem[CONSOLE_DATA], stdout);
    fflush(stdout);
    writeb(CONSOLE_CMD + CLI_CMD, CLI_CMD_IDLE);
  }
  
  if(readb(KEYBORD_CMD + KBD_CMD) == KBD_CMD_GETC) {
    (void) write(STDOUT_FILENO, "\033[?25h\033[1 q", 11);
    char c = getchar();
    (void) write(STDOUT_FILENO, "\033[?25l", 6);
    writeb(KEYBORD_DATA, c);
    writeb(KEYBORD_CMD + KBD_CMD, KBD_CMD_IDLE);
  }
  
}
