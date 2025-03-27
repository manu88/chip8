//
//  Compiler.hpp
//  asm
//
//  Created by Manuel Deneu on 25/03/2025.
//

#pragma once
#include "FileLoader.hpp"
#include <optional>
#include <string>

class Assembler {
  public:
    struct Error {
        int line;
        std::string msg;
    };

    using OptionalError = std::optional<Error>;

    Assembler();
    Assembler(const std::string &code);
    bool loadFile(const std::string &path);

    Chip8::Bytes generate();

    OptionalError getError() const { return _error; }

  private:
    uint16_t processLine(const std::string &line, OptionalError &error);
    std::string _code;

    OptionalError _error = std::nullopt;
};
