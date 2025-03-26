//
//  main.cpp
//  asm
//
//  Created by Manuel Deneu on 25/03/2025.
//

#include <iostream>
#include "Assembler.hpp"
#include "Disassembler.hpp"

int main(int argc, const char * argv[]) {
    Disassembler disassembler;
    disassembler.loadFile("/Users/manueldeneu/Documents/dev/chip8EMU/roms/helloworld.rom");
    disassembler.generate();
    return 0;
}
