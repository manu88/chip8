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
    const std::string c = "";
    Assembler asembler(c);
    asembler.generate();
    return 0;
}
