CC = riscv64-unknown-elf-gcc
CFLAGS = -nostdlib -ffreestanding -march=rv32im -mabi=ilp32 -mno-strict-align -mdiv -O0
LDFLAGS = -Ttext=0x1000

OBJCOPY = riscv64-unknown-elf-objcopy
OBJCFLAGS = 

OBJDUMP = riscv64-unknown-elf-objdump
OBJDFLAGS = -D -m riscv:rv32im

SRCDIR = src
BUILDDIR = build
BINDIR = bin

SOURCES = $(wildcard $(SRCDIR)/*.c)
OBJECTS = $(patsubst $(SRCDIR)/%.c, $(BUILDDIR)/%.o, $(SOURCES))

ELF = $(BUILDDIR)/mainelf.elf
BIN = $(BINDIR)/program.bin

all: $(BIN)

$(BIN): $(ELF)
	@mkdir -p $(BINDIR)
	$(OBJCOPY) $(OBJCFLAGS) -O binary $< $@

$(ELF): $(OBJECTS)
	@mkdir -p $(BUILDDIR)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

$(BUILDDIR)/%.o: $(SRCDIR)/%.c | $(BUILDDIR)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILDDIR) $(BINDIR)

disasm: $(ELF)
	$(OBJDUMP) $(OBJDFLAGS) $<

.PHONY: all clean disasm
