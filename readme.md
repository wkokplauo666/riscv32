# riscv32i
this project is a for-fun project made by a little  unexperienced developer wanting to know more about embedded systems, i'd say it worked.

## usage
you can build the emulator by simply running
```sh
make
```
this generates an executable located in build/rv32i, which then you can run by running
```sh
./build/rv32i [rom].. # change [rom] to your rom file
```
or, use the built-in rom file
```sh
./build/rv32i program/bin/program.bin
```
## note for developers
for developers, you can chdir to `program/` and make your own custom program to run on the emulator by running `make` (this  does nothing if you hadnt changed anything inside the `src/` folder)
i will not tell you how does my emulator works and how can devices like I/O interacts with it for now but `src/printf.c` (in the program/ folder) already implements basic I/O functions like putc, puts, and even printf
