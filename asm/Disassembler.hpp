//
//  Disassembler.hpp
//  asm
//
//  Created by Manuel Deneu on 25/03/2025.
//

#pragma once
#include <vector>
#include <stdint.h>
#include "FileLoader.hpp"

class Disassembler{
public:
    bool loadFile(const std::string &path);
    
    void generate();
private:
    Chip8::Bytes _bytes;
    
};
