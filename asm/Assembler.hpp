//
//  Compiler.hpp
//  asm
//
//  Created by Manuel Deneu on 25/03/2025.
//

#pragma once
#include <string>

class Assembler{
public:
    Assembler(const std::string &code);
    
    void generate();
private:
    const std::string _code;
};
