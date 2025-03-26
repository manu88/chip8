//
//  Compiler.cpp
//  asm
//
//  Created by Manuel Deneu on 25/03/2025.
//

#include "Assembler.hpp"
#include <algorithm>
#include <assert.h>
#include <cctype>
#include <fstream>
#include <locale>
#include <sstream>
#include <streambuf>
#include <string>

static void ltrim(std::string &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
                return !std::isspace(ch);
            }));
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

Assembler::Assembler(const std::string &code) : _code(code) {}
Assembler::Assembler() : Assembler("") {}

bool Assembler::loadFile(const std::string &path) {
    std::ifstream infile(path);
    if (!infile.good()) {
        return false;
    }
    _code = std::string((std::istreambuf_iterator<char>(infile)),
                        std::istreambuf_iterator<char>());
    return true;
}

Chip8::Bytes Assembler::generate() {
    Chip8::Bytes ret;

    std::istringstream iss(_code);
    int lineNum = 0;
    for (std::string line; std::getline(iss, line);) {
        rtrim(line);
        ltrim(line);
        removeComments(line);
        if (line.size() > 0) {

            uint16_t byte = processLine(line);
            if (byte == 0) {
                printf("Unable to generate output for line %i\n", lineNum);
                return {};
            }
            ret.push_back(byte);
        }
        lineNum++;
    }
    return ret;
}

struct Instruction {
    std::string op;
    std::vector<std::string> args;
};

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

Instruction splitInstruction(const std::string &line) {

    std::size_t found = line.find(" ");
    if (found == std::string::npos) {
        return {line};
    }
    const std::string strArgs = line.substr(found + 1);
    return {line.substr(0, found), splitArguments(strArgs)};
}

uint16_t generateFX29(const std::string &arg) {
    uint8_t reg = std::atoi(arg.c_str() + 1);
    uint16_t ret = 0XF029 + (reg << 8);
    return ret;
}

uint16_t generateFX18(const std::string &arg) {
    uint8_t reg = std::atoi(arg.c_str() + 1);
    uint16_t ret = 0XF018 + (reg << 8);
    return ret;
}

uint16_t parseNumber(const std::string &str) {
    return std::stoll(str.c_str(), nullptr, 0);
}

bool isNumber(const std::string &s) {
    if (s[0] == '0' && std::tolower(s[1]) == 'x') {
        auto sub = s.substr(2);
        return isNumber(sub);
    }
    return !s.empty() && std::find_if(s.begin(), s.end(), [](unsigned char c) {
                             return !std::isdigit(c);
                         }) == s.end();
}
uint16_t generateAnnn(const std::string &arg) {
    uint16_t val = parseNumber(arg);
    assert(val <= 0X0FFF);
    uint16_t ret = 0XA000 + val;
    return ret;
}

uint16_t generate8xy0(const std::string &arg0, const std::string &arg1) {
    uint8_t reg0 = std::atoi(arg0.c_str() + 1);
    uint8_t reg1 = std::atoi(arg1.c_str() + 1);
    uint16_t ret = 0x8000 + (reg0 << 8) + (reg1 << 4);
    return ret;
}

uint16_t generate6xkk(const std::string &arg0, const std::string &arg1) {
    uint8_t reg0 = std::atoi(arg0.c_str() + 1);
    uint16_t val = parseNumber(arg1);
    uint16_t ret = 0x6000 + (reg0 << 8) + val;
    return ret;
}

uint16_t generateFx0A(const std::string &arg0) {
    uint8_t reg0 = std::atoi(arg0.c_str() + 1);
    uint16_t ret = 0xF00A + (reg0 << 8);
    return ret;
}

uint16_t generateLDMachineCode(const std::vector<std::string> &args) {
    if (args.at(0) == "F") {
        // Fx29 - LD F, Vx
        return generateFX29(args[1]);
    } else if (args.at(0) == "ST") {
        // Fx18 - LD ST, Vx
        return generateFX18(args[1]);
    } else if (args.at(0) == "I") {
        // Annn - LD I, addr
        return generateAnnn(args[1]);
    } else if (std::tolower(args.at(0)[0]) == 'v') {
        if (std::tolower(args.at(1)[0]) == 'v') {
            // 8xy0 - LD Vx, Vy
            return generate8xy0(args[0], args[1]);
        } else if (std::tolower(args.at(1)[0]) == 'k') {
            // Fx0A - LD Vx, K
            return generateFx0A(args[0]);
        } else if (isNumber(args.at(1))) {
            // 6xkk - LD Vx, byte
            return generate6xkk(args[0], args[1]);
        }
        return 0;
    }
    return 0;
}

uint16_t generateDxyn(const std::vector<std::string> &args) {
    // Dxyn - DRW Vx, Vy, nibble
    uint8_t reg0 = std::atoi(args.at(0).c_str() + 1);
    uint8_t reg1 = std::atoi(args.at(1).c_str() + 1);
    uint8_t val = parseNumber(args.at(2));
    assert(val <= 0Xf);
    uint16_t ret = 0xD000 + (reg0 << 8) + (reg1 << 4) + val;
    return ret;
}

uint16_t generateJP(const std::vector<std::string> &args) {
    if (args.size() == 2) {
        // Bnnn - JP V0, addr
        return 0;
    } else if (isNumber(args[0])) {
        uint16_t addr = parseNumber(args[0]);
        assert(addr <= 0xFFF);
        // 1nnn - JP addr
        return 0x1000 + addr;
    }
    return 0;
}

uint16_t generateMachineCode(const Instruction &inst) {
    if (inst.op == "CLS") {
        return 0x00E0;
    } else if (inst.op == "RET") {
        return 0x00EE;
    } else if (inst.op == "LD") {
        return generateLDMachineCode(inst.args);
    } else if (inst.op == "DRW") {
        return generateDxyn(inst.args);
    } else if (inst.op == "JP") {
        return generateJP(inst.args);
    }
    return 0;
}

uint16_t Assembler::processLine(const std::string &line) {
    // printf("Line: '%s'\n", line.c_str());
    Instruction inst = splitInstruction(line);
    if (inst.op.empty()) {
        printf("Unable to parse OP from line '%s'\n", line.c_str());
        return 0;
    }

    return generateMachineCode(inst);
}
