//
//  Disassembler.hpp
//  asm
//
//  Created by Manuel Deneu on 25/03/2025.
//

#pragma once
#include <vector>
#include <stdint.h>

class Disassembler{
public:
    Disassembler(const std::vector<uint8_t> &bytes);
    
    void generate();
private:
    const std::vector<uint8_t> _bytes;
    
};
