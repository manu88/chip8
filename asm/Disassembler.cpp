//
//  Disassembler.cpp
//  asm
//
//  Created by Manuel Deneu on 25/03/2025.
//

#include "Disassembler.hpp"


bool Disassembler::loadFile(const std::string &path) {
    _bytes = Chip8::loadFile(path);
    return true;
}


void Disassembler::generate(){
    for(const auto &instr : _bytes){
        printf("Got 0X%0X\n", instr);
    }
}
