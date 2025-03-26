//
//  main.cpp
//  asm
//
//  Created by Manuel Deneu on 25/03/2025.
//

#include <iostream>
#include "Assembler.hpp"
#include "Disassembler.hpp"

int runAsembler(const std::string &binFilePath){
    Assembler assembler;
    assembler.loadFile(binFilePath);
    auto binary = assembler.generate();
    printf("--OUTPUT--\n");
    for (const auto b: binary){
        printf("0X%0X\n", b);
    }

    return 0;
}

int runDisassembler(const std::string &binFilePath){
    Disassembler disassembler;
    disassembler.loadFile(binFilePath);
    std::string code = disassembler.generate();
    printf("%s\n", code.c_str());
    return 0;
}

int main(int argc, const char * argv[]) {
    return runAsembler("/Users/manueldeneu/Documents/dev/chip8EMU/tests/hello.asm");
    //return runDisassembler("/Users/manueldeneu/Documents/dev/chip8EMU/roms/helloworld.rom");
}
