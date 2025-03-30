#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <termios.h>
#include <signal.h>

#include "../include/emu.h"
#include "memory.h"
#include "cpu.h"

FILE *stdlog = NULL;
emu_config_t g_config = {0};


void setbf(void);

void cleanup(void) {

  if(g_config.flg & ECT_FLG_MEMOK) {
    free(mem);
  }

  if(stdlog) fclose(stdlog);
  close(STDLOG_FILENO);

  if(g_config.flg & ECT_FLG_TERMBUF) {
    setbf();
  }
  
  printf("\033[?25h");
}

int set_stdlog(int type, const char *fname) {
  int log_fd;

  if(type == ECT_LTY_LOGSTDOUT && dup2(STDOUT_FILENO, STDLOG_FILENO) == -1) {
    perror("dup2");
    return 1;
  } else if(type == ECT_LTY_LOGFILE){
    log_fd = open(fname, O_WRONLY | O_CREAT | O_APPEND, 0644);
    if(log_fd == -1) {
      perror("open");
      return 1; 
    }
    
    if(dup2(log_fd, STDLOG_FILENO) == -1) {
      perror("dup2");
      close(log_fd);
      return 1;
    }
    
    close(log_fd);
  }

  stdlog = fdopen(STDLOG_FILENO, "a");
  if(!stdlog) {
    perror("fdopen");
    close(STDLOG_FILENO);
    return 1;
  }

  return 0; 
} 

int init(void) {
  if(init_mem() != 0) {
    fprintf(stderr, "Memory initialization failed\n");
    exit(1);
  }
  
  if(init_cpu() != 0) {
    fprintf(stderr, "CPU initialization failed\n");
    exit(1);
  }

  return 0;
}

long get_fsize(FILE *file) {
  long curpos = ftell(file);
  fseek(file, 0, SEEK_END);
  long end = ftell(file);
  fseek(file, curpos, SEEK_SET);
  return end;
}

int load_rom(const char *fname) {
  static uint16_t offset = 0x1000;
  FILE *file = fopen(fname, "rb");
  if(file == NULL) {
    perror("fopen");
    return 1;
  }

  uint32_t romsiz;
  if((romsiz = get_fsize(file)) > MEMSIZE) {
    fprintf(stderr, "Error loading %s: file too big (max size %d bytes)\n", fname, MEMSIZE);
    return 1;
  }
  
  if(offset + romsiz > MEMSIZE) {
    fprintf(stderr, "Error loading %s: insufficient memory size\n", fname);
    return 1;
  }

  if(fread(mem + offset, romsiz, 1, file) <= 0) {
    perror("fread failed");
    return 1;
  }
  offset += romsiz;

  fclose(file);
  return 0;
}

struct termios orig;

void unbuf(void) {
  struct termios t;

  tcgetattr(STDIN_FILENO, &orig);
  memcpy(&t, &orig, sizeof(struct termios));

  t.c_lflag &= ~(ICANON | ECHO);

  tcsetattr(STDIN_FILENO, TCSANOW, &t);

  g_config.flg |= ECT_FLG_TERMBUF;
}

void setbf(void) {
  tcsetattr(STDIN_FILENO, TCSANOW, &orig);
  g_config.flg &= ~(ECT_FLG_TERMBUF);
} 

void print_help(const char *progname) {
  fprintf(stderr, "Usage: \n");
  fprintf(stderr, "      %s [options] [roms]...\n", progname);
  fprintf(stderr, "Options:\n");
  fprintf(stderr, "    -d [f|s]   set the display type. f = framebuffer, and s = serial UART (default: f)\n"); // todo: implememnt this

  fprintf(stderr, "    -f fname   set the output log file name (if -l f)\n");
  fprintf(stderr, "    -h         print this summary.\n");
  fprintf(stderr, "    -l [o|f|n] set the log type. o = stdout, f = file, and n = none (default: n)\n");
  fprintf(stderr, "    -s         step mode\n");
  exit(1);
}


void siginth(int s) {
  (void)s;
  printf("\n");
  printf("\033[?25h");
  exit(1);
}

int main(int argc, char **argv) {
  int opt;

  char logfname[100] = {0};

  while((opt = getopt(argc, argv, ":hl:f:")) != -1) {
    switch(opt) {
      case 'l': {}
        char sl = optarg[0];
        if(sl == 'o') {
          g_config.logtype = ECT_LTY_LOGSTDOUT;
        } else if(sl == 'f') {
          g_config.logtype = ECT_LTY_LOGFILE;
        } else if(sl == 'n') {
          g_config.logtype = ECT_LTY_LOGNONE;
        } else {
          fprintf(stderr, "Invalid option '%c'. Avaliable options: o, f, n\n", sl);
        }

        break;
      case 'f':
        strncpy(logfname, optarg, sizeof logfname - 1);
        break;
      case 'h':
        print_help(argv[0]);
        break;
      case 's':
        g_config.flg |= ECT_FLG_EMUSTEP;
        break;
      case '?':
        fprintf(stderr, "Unknown option -%c\n", optopt);
        return 2;
        break;
      case ':':
        fprintf(stderr, "Option -%c needs an argument\n", optopt);
        return 2;
        break;
    }
  }
  
  if(g_config.logtype != ECT_LTY_LOGNONE) {

    if(g_config.logtype == ECT_LTY_LOGFILE && logfname[0] == '\0') {
      fprintf(stderr, "No logging filename specified. Using the default of log.txt...\n");
      strcpy(logfname, "log.txt");
    }

    if(set_stdlog(g_config.logtype, logfname) != 0) {
      fprintf(stderr, "stdlog set up failed (fd = %d)\n", STDLOG_FILENO);
      return 1;
    };
  } else {
    int fd = open("/dev/null", O_WRONLY, 0644);
    if(fd == -1) {
      perror("open");
      return 1;
    }

    if(dup2(fd, STDLOG_FILENO) == -1) {
      perror("dup2");
      close(fd);
      return 1;
    }

    stdlog = fdopen(STDLOG_FILENO, "w");
    if(!stdlog) {
      perror("fdopen");
      return 1;
    }
  }
  
  if(atexit(cleanup) != 0) {
    fprintf(stderr, "Failed to register atexit handler\n");
    return 1;
  }

  if(init() != 0) {
    fprintf(stderr, "Initialization failed\n");
    return 1;
  }
  
  int i;
  for(i = optind; i < argc; i++) {
    fprintf(stdlog, "Loading %s...\n", argv[i]);
    if(load_rom(argv[i]) != 0) {
      fprintf(stderr, "Error loading %s\n", argv[i]);
      exit(1);
    }
  }
  
  if(i == optind) {
    fprintf(stderr, "ROM file not specified\n");
    print_help(argv[0]);
  }
  
  signal(SIGINT, siginth);
  
  unbuf();
  if(g_config.flg & ECT_FLG_EMUSTEP) {
    printf("The terminal has been unbuffered. Press space to step, q to quit\n");
  } else {
    printf("The terminal has been unbuffered. CTRL+C to quit\n\n");
  }

  char c;
  if(g_config.logtype != ECT_LTY_LOGNONE) {
    print_dbg();
    while( !(g_config.flg & ECT_FLG_EMUSTEP) || (c = getchar()) != 'q') {
      step();
      print_dbg();
      fprintf(stdlog, "\n");
    }
  } else {
    while( !(g_config.flg & ECT_FLG_EMUSTEP) || (c = getchar()) != 'q') {
      step();
    }
  }
  
  setbf();
  return 0;
}
