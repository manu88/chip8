CXX := clang++

CPPFLAGS= -g -std=c++17
CPPFLAGS+=-I/opt/homebrew/include/ -Isrc/

LDFLAGS=-L/opt/homebrew/lib/ -lSDL2 -lSDL2_ttf -lncurses
LDLIBS=

SRCS_EMU=src/Assembler.cpp src/InstructionParser.cpp src/Rom.cpp src/Emulator.cpp src/Memory.cpp src/SDLPeripherals.cpp src/TermPeripherals.cpp src/FileLoader.cpp src/Peripherals.cpp src/main.cpp

SRCS_ASM=asm/Disassembler.cpp asm/main.cpp asm/Tests.cpp src/Assembler.cpp src/FileLoader.cpp src/InstructionParser.cpp



EMU_NAME=chip8EMU
OBJS_EMU=$(subst .cpp,.o,$(SRCS_EMU))

ASM_NAME = chip8ASM
OBJS_ASM=$(subst .cpp,.o,$(SRCS_ASM))

all: $(EMU_NAME) $(ASM_NAME)

$(EMU_NAME): $(OBJS_EMU)
	g++ $(LDFLAGS) -o $(EMU_NAME) $(OBJS_EMU) $(LDLIBS)

$(ASM_NAME): $(OBJS_ASM)
	g++ $(LDFLAGS) -o $(ASM_NAME) $(OBJS_ASM) $(LDLIBS)

clean:
	rm -f $(OBJS_EMU)
	rm -f $(OBJS_ASM)
	rm -rf $(EMU_NAME)
	rm -rf $(ASM_NAME)

.PHONY: clean all
