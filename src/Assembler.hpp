//
//  Compiler.hpp
//  asm
//
//  Created by Manuel Deneu on 25/03/2025.
//

#pragma once
#include "Config.hpp"
#include "FileLoader.hpp"
#include <map>
#include <optional>
#include <string>

class Assembler {
  public:
    struct Error {
        int line;
        std::string msg;
    };

    using OptionalError = std::optional<Error>;

    struct Instruction {
        std::string op;
        std::vector<std::string> args;
        int originalLineNum = 0;

        bool isLabel() const { return op[op.size() - 1] == ':'; }
        std::string getLabel() const { return op.substr(0, op.size() - 1); }

        std::string getCode() const {
            std::string ret = op;
            for (const std::string &arg : args) {
                ret += ", " + arg;
            }
            return ret;
        }
    };

    Assembler(const std::string &code, Chip8::Config conf = Chip8::Config())
        : _originalCode(code), _conf(conf) {}

    Chip8::Bytes generate();

    OptionalError getError() const { return _error; }

  private:
    bool preprocess();
    Chip8::Bytes process();
    uint16_t processLine(const std::string &line, OptionalError &error);
    uint16_t generateMachineCode(const Instruction &inst,
                                 Assembler::OptionalError &error);
    uint16_t generateLDMachineCode(const std::vector<std::string> &args,
                                   Assembler::OptionalError &error);
    uint16_t generateJP(const std::vector<std::string> &args,
                        Assembler::OptionalError &error);
    uint16_t generateDxyn(const std::vector<std::string> &args,
                          Assembler::OptionalError &error);
    uint16_t generate2nnn(const std::vector<std::string> &args,
                          Assembler::OptionalError &error);

    std::string _originalCode;
    std::vector<Instruction> _instructions;
    uint16_t _currentAddr = 0;

    OptionalError _error = std::nullopt;

    bool addLabel(const std::string &label);
    bool addLabel(const std::string &label, uint16_t addr);
    uint16_t getAddrForLabel(const std::string &label) const;
    std::map<std::string, uint16_t> _labels;

    Chip8::Config _conf;
};
