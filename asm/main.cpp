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
    std::string code = disassembler.generate();
    printf("%s\n", code.c_str());
    return 0;
}
