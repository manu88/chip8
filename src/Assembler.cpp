//
//  Compiler.cpp
//  asm
//
//  Created by Manuel Deneu on 25/03/2025.
//

#include "Assembler.hpp"
#include "HexHelpers.hpp"
#include "Memory.hpp"
#include <algorithm>
#include <assert.h>
#include <cctype>
#include <locale>
#include <sstream>
#include <streambuf>
#include <string>

static void ltrim(std::string &s) {
    auto pos = std::find_if(s.begin(), s.end(),
                            [](unsigned char ch) { return !std::isspace(ch); });
    s.erase(s.begin(), pos);
}

static void rtrim(std::string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(),
                         [](unsigned char ch) { return !std::isspace(ch); })
                .base(),
            s.end());
}

static void removeComments(std::string &s) {
    std::size_t found = s.find(";");
    if (found == std::string::npos) {
        return;
    }
    s.resize(found);
}

std::vector<std::string> splitArguments(const std::string &argsStr) {
    std::vector<std::string> args;
    std::istringstream f(argsStr);
    std::string s;
    while (getline(f, s, ',')) {
        ltrim(s);
        rtrim(s);
        args.push_back(s);
    }
    return args;
}

Assembler::Instruction splitInstruction(const std::string &line) {
    std::size_t found = line.find(" ");
    if (found == std::string::npos) {
        return {line};
    }
    const std::string strArgs = line.substr(found + 1);
    std::string op = line.substr(0, found);
    toUpper(op);
    return {op, splitArguments(strArgs)};
}

Chip8::Bytes Assembler::generate() {
    if (!preprocess()) {
        return {};
    }
    return process();
}

bool Assembler::preprocess() {
    _instructions.clear();
    std::istringstream iss(_originalCode);
    int lineNum = 1;
    uint16_t addr = Chip8::Memory::ROM_START;
    for (std::string line; std::getline(iss, line);) {
        rtrim(line);
        ltrim(line);
        toUpper(line);
        removeComments(line);
        if (!line.empty()) {
            Instruction inst = splitInstruction(line);
            inst.originalLineNum = lineNum;
            if (inst.op.empty()) {
                printf("Unable to parse OP from line '%s'\n", line.c_str());
                return false;
            }
            if (inst.isLabel()) {
                if (!addLabel(inst.getLabel(), addr)) {
                    printf("duplicate label'%s'\n", inst.getLabel().c_str());
                    return false;
                }
            }
            _instructions.push_back(inst);
            addr += 2;
        }
        lineNum++;
    }
    return true;
}

Chip8::Bytes Assembler::process() {
    Chip8::Bytes ret;
    for (const auto &inst : _instructions) {
        OptionalError err = std::nullopt;
        uint16_t byteCode = generateMachineCode(inst, err);
        if (err) {
            _error = err;
            _error.value().line = inst.originalLineNum;
            return {};
        }
        if (byteCode == 0) {
            continue;
        }
        uint8_t b0 = byteCode >> 8;
        uint8_t b1 = (uint8_t)byteCode;
        ret.push_back(b0);
        ret.push_back(b1);
        _currentAddr += 2;
    }
    return ret;
}

uint16_t generateFX29(const std::string &arg, Assembler::OptionalError &error) {
    bool valid = false;
    uint8_t reg = parseRegisterAddr(arg, valid);
    if (!valid) {
        error = {.msg = "invalid register address '" + arg + "'"};
        return 0;
    }
    uint16_t ret = 0XF029 + (reg << 8);
    return ret;
}

uint16_t generateFX33(const std::string &arg, Assembler::OptionalError &error) {
    bool valid = false;
    uint8_t reg = parseRegisterAddr(arg, valid);
    if (!valid) {
        error = {.msg = "invalid register address '" + arg + "'"};
        return 0;
    }
    uint16_t ret = 0XF033 + (reg << 8);
    return ret;
}

uint16_t generateFX18(const std::string &arg, Assembler::OptionalError &error) {
    bool valid = false;
    uint8_t reg = parseRegisterAddr(arg, valid);
    if (!valid) {
        error = {.msg = "invalid register address '" + arg + "'"};
        return 0;
    }
    uint16_t ret = 0XF018 + (reg << 8);
    return ret;
}

uint16_t generateAnnn(const std::string &arg, Assembler::OptionalError &error) {
    bool argValid = false;
    uint16_t val = parseAddr(arg, argValid);
    if (!argValid) {
        error = {.msg = "invalid value '" + arg + "'"};
        return 0;
    }
    assert(val <= 0X0FFF);
    uint16_t ret = 0XA000 + val;
    return ret;
}

uint16_t generateFx07(const std::string &arg0,
                      Assembler::OptionalError &error) {

    bool valid = false;
    uint8_t reg0 = parseRegisterAddr(arg0, valid);
    if (!valid) {
        error = {.msg = "invalid register address '" + arg0 + "'"};
        return 0;
    }
    uint16_t ret = 0xF007 + (reg0 << 8);
    return ret;
}

uint16_t generateFx15(const std::string &arg1,
                      Assembler::OptionalError &error) {
    bool valid = false;
    uint8_t reg0 = parseRegisterAddr(arg1, valid);
    if (!valid) {
        error = {.msg = "invalid register address '" + arg1 + "'"};
        return 0;
    }
    uint16_t ret = 0xF015 + (reg0 << 8);
    return ret;
}

uint16_t generate8xy0(const std::string &arg0, const std::string &arg1,
                      Assembler::OptionalError &error) {
    bool valid = false;
    uint8_t reg0 = parseRegisterAddr(arg0, valid);
    if (!valid) {
        error = {.msg = "invalid register address '" + arg0 + "'"};
        return 0;
    }
    valid = false;
    uint8_t reg1 = parseRegisterAddr(arg1, valid);
    if (!valid) {
        error = {.msg = "invalid register address '" + arg0 + "'"};
        return 0;
    }
    uint16_t ret = 0x8000 + (reg0 << 8) + (reg1 << 4);
    return ret;
}

uint16_t generate6xkk(const std::string &arg0, const std::string &arg1,
                      Assembler::OptionalError &error) {
    bool argValid = false;
    uint8_t reg0 = parseRegisterAddr(arg0, argValid);
    if (!argValid) {
        error = {.msg = "invalid value '" + arg0 + "'"};
        return 0;
    }
    argValid = false;
    uint16_t val = parseByte(arg1, argValid);
    if (!argValid) {
        error = {.msg = "invalid value '" + arg1 + "'"};
        return 0;
    }
    uint16_t ret = 0x6000 + (reg0 << 8) + val;
    return ret;
}

uint16_t generateFx0A(const std::string &arg0,
                      Assembler::OptionalError &error) {
    bool valid = false;
    uint8_t reg0 = parseRegisterAddr(arg0, valid);
    if (!valid) {
        error = {.msg = "invalid value '" + arg0 + "'"};
        return 0;
    }
    uint16_t ret = 0xF00A + (reg0 << 8);
    return ret;
}

uint16_t generatefx55(const std::string &arg0,
                      Assembler::OptionalError &error) {
    bool valid = false;
    uint8_t reg0 = parseRegisterAddr(arg0, valid);
    if (!valid) {
        error = {.msg = "invalid value '" + arg0 + "'"};
        return 0;
    }
    // Fx55 - LD [I], Vx
    uint16_t ret = 0xF055 + (reg0 << 8);
    return ret;
}

uint16_t generateFx65(const std::string &arg0,
                      Assembler::OptionalError &error) {
    bool valid = false;
    uint8_t reg0 = parseRegisterAddr(arg0, valid);
    if (!valid) {
        error = {.msg = "invalid value '" + arg0 + "'"};
        return 0;
    }
    uint16_t ret = 0xF065 + (reg0 << 8);
    return ret;
}

uint16_t generateFx33(const std::string &arg1,
                      Assembler::OptionalError &error) {
    bool valid = false;
    uint8_t reg0 = parseRegisterAddr(arg1, valid);
    if (!valid) {
        error = {.msg = "invalid value '" + arg1 + "'"};
        return 0;
    }
    uint16_t ret = 0xF033 + (reg0 << 8);
    return ret;
}

uint16_t generateFx30(const std::string &arg0,
                      Assembler::OptionalError &error) {
    bool valid = false;
    uint8_t reg0 = parseRegisterAddr(arg0, valid);
    if (!valid) {
        error = {.msg = "invalid value '" + arg0 + "'"};
        return 0;
    }
    uint16_t ret = 0XF030 + (reg0 << 8);
    return ret;
}

uint16_t generateFx75(const std::string arg1, Assembler::OptionalError &error) {
    // Fx75 - LD R, Vx
    bool valid = false;
    uint8_t reg0 = parseRegisterAddr(arg1, valid);
    if (!valid) {
        error = {.msg = "invalid value '" + arg1 + "'"};
        return 0;
    }
    if (reg0 > 7) {
        error = {.msg = "invalid reg " + arg1 + ". must be 0..7"};
        return 0;
    }
    uint16_t ret = 0XF075 + (reg0 << 8);
    return ret;
}

uint16_t generateFx85(const std::string arg0, Assembler::OptionalError &error) {
    // Fx85 - LD Vx, R
    bool valid = false;
    uint8_t reg0 = parseRegisterAddr(arg0, valid);
    if (!valid) {
        error = {.msg = "invalid value '" + arg0 + "'"};
        return 0;
    }
    if (reg0 > 7) {
        error = {.msg = "invalid reg " + arg0 + ". must be 0..7"};
        return 0;
    }
    uint16_t ret = 0XF085 + (reg0 << 8);
    return ret;
}

uint16_t Assembler::generateLDMachineCode(const std::vector<std::string> &args,
                                          Assembler::OptionalError &error) {
    if (args.size() != 2) {
        error = {.msg = "invalid number of arguments"};
        return 0;
    }
    if (args.at(0) == "F") {
        if (args.size() != 2) {
            error = {.msg = "missing arguments"};
            return 0;
        }
        // Fx29 - LD F, Vx
        return generateFX29(args[1], error);
    } else if (args.at(0) == "ST") {
        // Fx18 - LD ST, Vx
        return generateFX18(args[1], error);
    } else if (args.at(0) == "B") {
        // Fx18 - LD B, Vx
        return generateFX33(args[1], error);
    } else if (args.at(0) == "I") {
        if (std::tolower(args.at(1)[0]) == 'v') {
            // Fx55 - LD [I], Vx
            return generatefx55(args[1], error);
        } else {
            // Annn - LD I, addr
            return generateAnnn(args[1], error);
        }
    } else if (args.at(0) == "DT") {
        // LD DT, Vx
        return generateFx15(args.at(1), error);
    } else if (std::tolower(args.at(0)[0]) == 'v') {
        if (std::tolower(args.at(1)[0]) == 'v') {
            // 8xy0 - LD Vx, Vy
            return generate8xy0(args[0], args[1], error);
        } else if (args.at(1) == "DT") {
            // Fx07 - LD Vx, DT
            return generateFx07(args[0], error);
        } else if (std::tolower(args.at(1)[0]) == 'k') {
            // Fx0A - LD Vx, K
            return generateFx0A(args[0], error);
        } else if (isNumber(args.at(1))) {
            // 6xkk - LD Vx, byte
            return generate6xkk(args[0], args[1], error);
        } else if (args.at(1) == "I") {
            // Fx65 - LD Vx, I
            return generateFx65(args[0], error);
        } else if (_conf.superInstructions && args.at(1) == "R") {
            return generateFx85(args.at(0), error);
        }
        error = {.msg = "unknown argument " + args[1]};
        return 0;
    } else if (args.at(0) == "B") {
        // Fx33 - LD B, Vx
        return generateFx33(args[1], error);
    } else if (_conf.superInstructions) {
        if (args.at(0) == "HF") {
            return generateFx30(args.at(1), error);
        } else if (args.at(0) == "R") {
            return generateFx75(args.at(1), error);
        }
    }
    error = {.msg = "unknown '" + args.at(0) + "' LD argument"};
    return 0;
}

uint16_t Assembler::generateDxyn(const std::vector<std::string> &args,
                                 Assembler::OptionalError &error) {
    // Dxyn - DRW Vx, Vy, nibble
    if (args.size() != 3) {
        error = {.msg = "invalid number of arguments"};
        return 0;
    }
    bool valid = false;
    uint8_t reg0 = parseRegisterAddr(args.at(0), valid);
    if (!valid) {
        error = {.msg = "invalid register address '" + args.at(0) + "'"};
        return 0;
    }
    valid = false;
    uint8_t reg1 = parseRegisterAddr(args.at(1), valid);
    if (!valid) {
        error = {.msg = "invalid register address '" + args.at(1) + "'"};
        return 0;
    }
    valid = false;
    uint8_t val = parseNibble(args.at(2), valid);
    if (!valid) {
        error = {.msg = "invalid value '" + args[2] + "'"};
        return 0;
    }
    assert(val <= 0Xf);
    if (val == 0 && !_conf.superInstructions) {
        error = {.msg = "special value 0 is for super chip only"};
        return 0;
    }
    uint16_t ret = 0xD000 + (reg0 << 8) + (reg1 << 4) + val;
    return ret;
}

uint16_t generate5xy0(const std::string &arg0, const std::string &arg1,
                      Assembler::OptionalError &error) {
    bool valid = false;
    uint8_t reg0 = parseRegisterAddr(arg0, valid);
    if (!valid) {
        error = {.msg = "invalid register address '" + arg0 + "'"};
        return 0;
    }
    valid = false;
    uint8_t reg1 = parseRegisterAddr(arg1, valid);
    if (!valid) {
        error = {.msg = "invalid register address '" + arg0 + "'"};
        return 0;
    }
    uint16_t ret = 0x5000 + (reg0 << 8) + (reg1 << 4);
    return ret;
}

uint16_t generate3xkk(const std::string &arg0, const std::string &arg1,
                      Assembler::OptionalError &error) {
    bool valid = false;
    uint8_t reg0 = parseRegisterAddr(arg0, valid);
    if (!valid) {
        error = {.msg = "invalid register address '" + arg0 + "'"};
        return 0;
    }

    valid = false;
    uint8_t val = parseByte(arg1, valid);
    if (!valid) {
        error = {.msg = "invalid value '" + arg1 + "'"};
        return 0;
    }
    assert(val <= 0XFF);
    return 0x3000 + (reg0 << 8) + val;
}

uint16_t generateSKP(const std::vector<std::string> &args,
                     Assembler::OptionalError &error) {
    if (args.size() != 1) {
        error = {.msg = "invalid number of arguments"};
        return 0;
    }
    bool valid = false;
    uint8_t reg0 = parseRegisterAddr(args.at(0), valid);
    if (!valid) {
        error = {.msg = "invalid register address '" + args.at(0) + "'"};
        return 0;
    }
    return 0xE09E + (reg0 << 8);
}

uint16_t generateSKNP(const std::vector<std::string> &args,
                      Assembler::OptionalError &error) {
    if (args.size() != 1) {
        error = {.msg = "invalid number of arguments"};
        return 0;
    }
    bool valid = false;
    uint8_t reg0 = parseRegisterAddr(args.at(0), valid);
    if (!valid) {
        error = {.msg = "invalid register address '" + args.at(0) + "'"};
        return 0;
    }
    return 0xE0A1 + (reg0 << 8);
}

uint16_t generate8xy4(const std::string &arg0, const std::string &arg1,
                      Assembler::OptionalError &error) {
    bool valid = false;
    uint8_t reg0 = parseRegisterAddr(arg0, valid);
    if (!valid) {
        error = {.msg = "invalid register address '" + arg0 + "'"};
        return 0;
    }
    valid = false;
    uint8_t reg1 = parseRegisterAddr(arg1, valid);
    if (!valid) {
        error = {.msg = "invalid register address '" + arg0 + "'"};
        return 0;
    }
    uint16_t ret = 0x8004 + (reg0 << 8) + (reg1 << 4);
    return ret;
}

uint16_t generate7xkk(const std::string &arg0, const std::string &arg1,
                      Assembler::OptionalError &error) {
    bool argValid = false;
    uint8_t reg0 = parseRegisterAddr(arg0, argValid);
    if (!argValid) {
        error = {.msg = "invalid value '" + arg0 + "'"};
        return 0;
    }
    argValid = false;
    uint16_t val = parseByte(arg1, argValid);
    if (!argValid) {
        error = {.msg = "invalid value '" + arg1 + "'"};
        return 0;
    }
    uint16_t ret = 0x7000 + (reg0 << 8) + val;
    return ret;
}

uint16_t generateFx1E(const std::string &arg, Assembler::OptionalError &error) {
    bool valid = false;
    uint8_t reg = parseRegisterAddr(arg, valid);
    if (!valid) {
        error = {.msg = "invalid register address '" + arg + "'"};
        return 0;
    }
    uint16_t ret = 0XF01E + (reg << 8);
    return ret;
}

uint16_t generateADD(const std::vector<std::string> &args,
                     Assembler::OptionalError &error) {
    if (args.size() != 2) {
        error = {.msg = "invalid number of arguments"};
        return 0;
    }
    if (std::tolower(args.at(0)[0]) == 'v') {
        if (std::tolower(args.at(1)[0]) == 'v') {
            //  8xy4 - ADD Vx, Vy
            return generate8xy4(args.at(0), args.at(1), error);
        } else {
            //  7xkk - ADD Vx, byte
            return generate7xkk(args.at(0), args.at(1), error);
        }
    } else if (args.at(0) == "I") {
        // Fx1E - ADD I, Vx
        return generateFx1E(args.at(1), error);
    }
    error = {.msg = "unexpected value '" + args.at(0) + "'"};
    return 0;
}

uint16_t Assembler::generate2nnn(const std::vector<std::string> &args,
                                 Assembler::OptionalError &error) {
    if (args.size() != 1) {
        error = {.msg = "invalid number of arguments"};
        return 0;
    }

    uint16_t addr = 0;
    if (isNumber(args.at(0))) {
        bool addrValid = false;
        addr = parseAddr(args.at(0), addrValid);
        if (!addrValid) {
            error = {.msg = "invalid value '" + args.at(0) + "'"};
            return 0;
        }
    } else if (_labels.count(args.at(0))) {
        addr = _labels.at(args.at(0));
    } else {
        error = {.msg = "invalid value '" + args.at(0) + "'"};
        return 0;
    }

    // 2nnn - CALL addr
    uint16_t ret = 0x2000 + addr;
    return ret;
}

uint16_t generate0nnn(const std::vector<std::string> &args,
                      Assembler::OptionalError &error) {
    if (args.size() != 1) {
        error = {.msg = "invalid number of arguments"};
        return 0;
    }
    bool addrValid = false;
    uint16_t val = parseAddr(args.at(0), addrValid);
    if (!addrValid) {
        error = {.msg = "invalid value '" + args.at(0) + "'"};
        return 0;
    }
    assert(val <= 0XFFF);
    return val;
}

uint16_t generateCxkk(const std::vector<std::string> &args,
                      Assembler::OptionalError &error) {
    if (args.size() != 2) {
        error = {.msg = "invalid number of arguments"};
        return 0;
    }
    bool valid = false;
    uint8_t reg0 = parseRegisterAddr(args.at(0), valid);
    if (!valid) {
        error = {.msg = "invalid register address '" + args.at(0) + "'"};
        return 0;
    }

    valid = false;
    uint8_t val = parseByte(args.at(1), valid);
    if (!valid) {
        error = {.msg = "invalid value '" + args.at(1) + "'"};
        return 0;
    }
    assert(val <= 0XFF);
    return 0xC000 + (reg0 << 8) + val;
    // Cxkk -  RND Vx, byte
    return 0;
}

uint16_t generateSE(const std::vector<std::string> &args,
                    Assembler::OptionalError &error) {
    if (args.size() != 2) {
        error = {.msg = "invalid number of arguments"};
        return 0;
    }
    if (std::tolower(args.at(1)[0]) == 'v') {
        // 5xy0 - SE Vx, Vy
        return generate5xy0(args[0], args[1], error);
    } else {
        // 3xkk - SE Vx, byte
        return generate3xkk(args[0], args[1], error);
    }
    return 0;
}

uint16_t generateSNE(const std::vector<std::string> &args,
                     Assembler::OptionalError &error) {
    if (args.size() != 2) {
        error = {.msg = "invalid number of arguments"};
        return 0;
    }
    bool valid = false;
    uint8_t reg0 = parseRegisterAddr(args.at(0), valid);
    if (!valid) {
        error = {.msg = "invalid register address '" + args.at(0) + "'"};
        return 0;
    }
    if (std::tolower(args.at(1)[0]) == 'v') {
        // 9xy0 - SNE Vx, Vy
        valid = false;
        uint8_t reg1 = parseRegisterAddr(args.at(1), valid);
        if (!valid) {
            error = {.msg = "invalid register address '" + args.at(1) + "'"};
            return 0;
        }
        return 0x9000 + (reg0 << 8) + (reg1 << 4);
    } else {
        valid = false;
        uint16_t val = parseByte(args[1], valid);
        if (!valid) {
            error = {.msg = "invalid value '" + args[1] + "'"};
            return 0;
        }
        assert(val <= 0x0FF);
        // 4xkk - SNE Vx, byte
        return 0x4000 + (reg0 << 8) + val;
    }
}

uint16_t generateAND(const std::vector<std::string> &args,
                     Assembler::OptionalError &error) {
    if (args.size() != 2) {
        error = {.msg = "invalid number of arguments, expected 2"};
        return 0;
    }
    bool valid = false;
    uint8_t reg0 = parseRegisterAddr(args.at(0), valid);
    if (!valid) {
        error = {.msg = "invalid register address '" + args.at(0) + "'"};
        return 0;
    }
    valid = false;
    uint8_t reg1 = parseRegisterAddr(args.at(1), valid);
    if (!valid) {
        error = {.msg = "invalid register address '" + args.at(1) + "'"};
        return 0;
    }
    // 8xy2 - AND Vx, Vy
    uint16_t ret = 0x8002 + (reg0 << 8) + (reg1 << 4);
    return ret;
}

uint16_t generateSUBN(const std::vector<std::string> &args,
                      Assembler::OptionalError &error) {
    if (args.size() != 2) {
        error = {.msg = "invalid number of arguments, expected 2"};
        return 0;
    }
    bool valid = false;
    uint8_t reg0 = parseRegisterAddr(args.at(0), valid);
    if (!valid) {
        error = {.msg = "invalid register address '" + args.at(0) + "'"};
        return 0;
    }
    valid = false;
    uint8_t reg1 = parseRegisterAddr(args.at(1), valid);
    if (!valid) {
        error = {.msg = "invalid register address '" + args.at(1) + "'"};
        return 0;
    }
    // 8xy7 - SUBN Vx, Vy
    uint16_t ret = 0x8007 + (reg0 << 8) + (reg1 << 4);
    return ret;
}

uint16_t generateSUB(const std::vector<std::string> &args,
                     Assembler::OptionalError &error) {
    if (args.size() != 2) {
        error = {.msg = "invalid number of arguments, expected 2"};
        return 0;
    }
    bool valid = false;
    uint8_t reg0 = parseRegisterAddr(args.at(0), valid);
    if (!valid) {
        error = {.msg = "invalid register address '" + args.at(0) + "'"};
        return 0;
    }
    valid = false;
    uint8_t reg1 = parseRegisterAddr(args.at(1), valid);
    if (!valid) {
        error = {.msg = "invalid register address '" + args.at(1) + "'"};
        return 0;
    }
    // 8xy5 - SUB Vx, Vy
    uint16_t ret = 0x8005 + (reg0 << 8) + (reg1 << 4);
    return ret;
}

uint16_t generateSHL(const std::vector<std::string> &args,
                     Assembler::OptionalError &error) {
    if (args.size() != 1) {
        error = {.msg = "invalid number of arguments, expected 1"};
        return 0;
    }
    bool valid = false;
    uint8_t reg0 = parseRegisterAddr(args.at(0), valid);
    if (!valid) {
        error = {.msg = "invalid register address '" + args.at(0) + "'"};
        return 0;
    }

    /*https://en.wikipedia.org/wiki/CHIP-8#cite_note-bitshift-26
     CHIP-8's opcodes 8XY6 and 8XYE (the bit shift instructions), which were in
     fact undocumented opcodes in the original interpreter, shifted the value in
     the register VY and stored the result in VX. The CHIP-48 and SCHIP
     implementations instead ignored VY, and simply shifted VX
     */

    // 8xyE - SHL Vx {, Vy}
    uint16_t ret = 0x800E + (reg0 << 8);
    return ret;
}

uint16_t generateSHR(const std::vector<std::string> &args,
                     Assembler::OptionalError &error) {
    if (args.size() != 1) {
        error = {.msg = "invalid number of arguments, expected 1"};
        return 0;
    }
    bool valid = false;
    uint8_t reg0 = parseRegisterAddr(args.at(0), valid);
    if (!valid) {
        error = {.msg = "invalid register address '" + args.at(0) + "'"};
        return 0;
    }

    /*https://en.wikipedia.org/wiki/CHIP-8#cite_note-bitshift-26
     CHIP-8's opcodes 8XY6 and 8XYE (the bit shift instructions), which were in
     fact undocumented opcodes in the original interpreter, shifted the value in
     the register VY and stored the result in VX. The CHIP-48 and SCHIP
     implementations instead ignored VY, and simply shifted VX
     */

    // 8xy6 - SHR Vx {, Vy}
    uint16_t ret = 0x8006 + (reg0 << 8);
    return ret;
}

uint16_t generateXOR(const std::vector<std::string> &args,
                     Assembler::OptionalError &error) {
    if (args.size() != 2) {
        error = {.msg = "invalid number of arguments, expected 2"};
        return 0;
    }
    bool valid = false;
    uint8_t reg0 = parseRegisterAddr(args.at(0), valid);
    if (!valid) {
        error = {.msg = "invalid register address '" + args.at(0) + "'"};
        return 0;
    }
    valid = false;
    uint8_t reg1 = parseRegisterAddr(args.at(1), valid);
    if (!valid) {
        error = {.msg = "invalid register address '" + args.at(1) + "'"};
        return 0;
    }
    // 8xy3 - XOR Vx, Vy
    uint16_t ret = 0x8003 + (reg0 << 8) + (reg1 << 4);
    return ret;
}

uint16_t generateOR(const std::vector<std::string> &args,
                    Assembler::OptionalError &error) {
    if (args.size() != 2) {
        error = {.msg = "invalid number of arguments, expected 2"};
        return 0;
    }
    bool valid = false;
    uint8_t reg0 = parseRegisterAddr(args.at(0), valid);
    if (!valid) {
        error = {.msg = "invalid register address '" + args.at(0) + "'"};
        return 0;
    }
    valid = false;
    uint8_t reg1 = parseRegisterAddr(args.at(1), valid);
    if (!valid) {
        error = {.msg = "invalid register address '" + args.at(1) + "'"};
        return 0;
    }
    // 8xy1 - OR Vx, Vy
    uint16_t ret = 0x8001 + (reg0 << 8) + (reg1 << 4);
    return ret;
}

uint16_t Assembler::generateJP(const std::vector<std::string> &args,
                               Assembler::OptionalError &error) {

    if (args.size() < 1) {
        error = {.msg = "invalid number of arguments"};
        return 0;
    }
    if (args.size() == 2 && args[0] == "V0") {
        // Bnnn - JP V0, addr
        // note: only V0 addr is valid,
        bool addrValid = false;
        uint16_t addr = parseAddr(args[1], addrValid);
        if (!addrValid) {
            error = {.msg = "invalid value '" + args[1] + "'"};
            return 0;
        }
        assert(addr <= 0xFFF);
        // bnnn - JP addr
        return 0xB000 + addr;
    } else if (isNumber(args[0]) || _labels.count(args[0])) {
        uint16_t addr = 0;
        if (_labels.count(args[0])) {
            addr = _labels.at(args[0]);
        } else {
            bool addrValid = false;
            addr = parseAddr(args[0], addrValid);
            if (!addrValid) {
                error = {.msg = "invalid value '" + args[1] + "'"};
                return 0;
            }
        }
        assert(addr <= 0xFFF);
        // 1nnn - JP addr
        return 0x1000 + addr;
    }
    error = {.msg = "unexpected value '" + args[0] + "'"};
    return 0;
}

uint16_t generateSCD(const std::vector<std::string> &args,
                     Assembler::OptionalError &error) {
    if (args.size() != 1) {
        error = {.msg = "invalid number of arguments"};
        return 0;
    }
    bool valid = false;
    // 00Cn
    uint8_t n = parseNibble(args.at(0), valid);
    if (!valid) {
        error = {.msg = "invalid value '" + args[0] + "'"};
        return 0;
    }
    return 0X00C0 + n;
}

uint16_t Assembler::generateMachineCode(const Instruction &inst,
                                        Assembler::OptionalError &error) {
    if (inst.op == "CLS") {
        return 0x00E0;
    } else if (inst.op == "RET") {
        return 0x00EE;
    } else if (inst.op == "LD") {
        return generateLDMachineCode(inst.args, error);
    } else if (inst.op == "DRW") {
        return generateDxyn(inst.args, error);
    } else if (inst.op == "OR") {
        return generateOR(inst.args, error);
    } else if (inst.op == "XOR") {
        return generateXOR(inst.args, error);
    } else if (inst.op == "SHR") {
        return generateSHR(inst.args, error);
    } else if (inst.op == "SHL") {
        return generateSHL(inst.args, error);
    } else if (inst.op == "SUBN") {
        return generateSUBN(inst.args, error);
    } else if (inst.op == "SUB") {
        return generateSUB(inst.args, error);
    } else if (inst.op == "AND") {
        return generateAND(inst.args, error);
    } else if (inst.op == "JP") {
        return generateJP(inst.args, error);
    } else if (inst.op == "SE") {
        return generateSE(inst.args, error);
    } else if (inst.op == "SNE") {
        return generateSNE(inst.args, error);
    } else if (inst.op == "SKNP") {
        return generateSKNP(inst.args, error);
    } else if (inst.op == "ADD") {
        return generateADD(inst.args, error);
    } else if (inst.op == "CALL") {
        return generate2nnn(inst.args, error);
    } else if (inst.op == "SYS") {
        return generate0nnn(inst.args, error);
    } else if (inst.op == "SKP") {
        return generateSKP(inst.args, error);
    } else if (inst.op == "RND") {
        return generateCxkk(inst.args, error);
    } else if (inst.isLabel()) {
        // ignore labels, they are already taken care of by preprocess
        return 0;
    } else if (_conf.superInstructions) {
        if (inst.op == "EXIT") {
            return 0X00FD;
        } else if (inst.op == "SCR") {
            return 0X00FB;
        } else if (inst.op == "SCL") {
            return 0X00FC;
        } else if (inst.op == "LOW") {
            return 0X00FE;
        } else if (inst.op == "HIGH") {
            return 0X00FF;
        } else if (inst.op == "SCD") {
            return generateSCD(inst.args, error);
        }
    }
    error = {.msg = "unrecognized instruction mnemonic '" + inst.op + "'"};
    return 0;
}

uint16_t Assembler::processLine(const std::string &line,
                                Assembler::OptionalError &error) {
    Instruction inst = splitInstruction(line);
    if (inst.op.empty()) {
        printf("Unable to parse OP from line '%s'\n", line.c_str());
        return 0;
    }

    return generateMachineCode(inst, error);
}

bool Assembler::addLabel(const std::string &label, uint16_t addr) {
    if (_labels.count(label) != 0) {
        return false;
    }
    _labels[label] = addr;
    return true;
}

bool Assembler::addLabel(const std::string &label) {
    return addLabel(label, _currentAddr);
}

uint16_t Assembler::getAddrForLabel(const std::string &label) const {
    return 0;
}
