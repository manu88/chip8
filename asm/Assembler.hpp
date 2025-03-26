//
//  Compiler.hpp
//  asm
//
//  Created by Manuel Deneu on 25/03/2025.
//

#pragma once
#include "FileLoader.hpp"
#include <string>

class Assembler {
  public:
    Assembler();
    Assembler(const std::string &code);
    bool loadFile(const std::string &path);

    Chip8::Bytes generate();

  private:
    uint16_t processLine(const std::string &line);
    std::string _code;
};
