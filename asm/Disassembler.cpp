//
//  Disassembler.cpp
//  asm
//
//  Created by Manuel Deneu on 25/03/2025.
//

#include "Disassembler.hpp"


static std::string hex(uint16_t value) {
    char str[16];
    char *p = &str[16];
    do {
        p--;
        uint32_t digit = value % 16;
        value /= 16;
        *p = digit >= 10 ? 'a' + (digit - 10) : '0' + digit;
    } while (value > 0);
    p--;
    *p = 'x';
    p--;
    *p = '0';
    return std::string(p, &str[16] - p);
}

bool Disassembler::loadFile(const std::string &path) {
    _bytes = Chip8::loadFile(path);
    return true;
}

std::string Disassembler::generate() {
    _text = "; autogenerated from chip8 disassembler\n";
    for (const auto &instr : _bytes) {
        if (!exec(instr)) {
            printf("Error at instruction 0X%0X\n", instr);
            return "";
        }
        _text += "\n";
    }
    return _text;
}

bool Disassembler::onCLS() { _text += "CLS"; return true;}
bool Disassembler::onRET() { _text += "CLS";return true; }
bool Disassembler::onCallMachine(uint16_t addr) { return false;}
bool Disassembler::onJump(uint16_t addr) {return false;}
bool Disassembler::onCallSubroutine(uint16_t addr) {return false;}
bool Disassembler::onSkipIfVxIsVal(uint16_t reg, uint16_t val) {return false;}
bool Disassembler::onSkipIfVxIsNotVal(uint16_t reg, uint16_t val) {return false;}
bool Disassembler::onSkipIfVxIsVy(uint16_t regX, uint16_t regY) {return false;}
bool Disassembler::onSetVx(uint16_t reg, uint16_t val) {
    _text += "mov v" + std::to_string(reg) + ", " + hex(val);
    return true;
}
bool Disassembler::onAddValToVx(uint16_t reg, uint16_t val) {return false;}
bool Disassembler::onSetVxToVy(uint16_t regX, uint16_t regY) {return false;}
bool Disassembler::onOrValToVx(uint16_t reg, uint16_t val) {return false;}
bool Disassembler::onAndValToVx(uint16_t reg, uint16_t val) {return false;}
bool Disassembler::onXOrValToVx(uint16_t reg, uint16_t val) {return false;}
bool Disassembler::onAddVyToVx(uint16_t regX, uint16_t regY) {return false;}
bool Disassembler::onSubVyToVx(uint16_t regX, uint16_t regY) {return false;}
bool Disassembler::onShiftRightVx(uint16_t reg) {return false;}
bool Disassembler::onSubVxToVy(uint16_t regX, uint16_t regY) {return false;}
bool Disassembler::onShiftLeftVx(uint16_t reg) {return false;}
bool Disassembler::onSkipNextIfVxIsNotVy(uint16_t regX, uint16_t regY) {return false;}
bool Disassembler::onSetI(uint16_t addr) {return false;}
bool Disassembler::onJumpToLoc(uint16_t val) {return false;}
bool Disassembler::onRand(uint16_t reg, uint16_t val) {return false;}
bool Disassembler::onDisplay(uint16_t regX, uint16_t regY, uint8_t nimble) {return false;}
bool Disassembler::onSkipIfKeyPressed(uint16_t reg) {return false;}
bool Disassembler::onSkipIfKeyNotPressed(uint16_t reg) {return false;}
bool Disassembler::onSetVxToDelayTimer(uint16_t reg) {return false;}
bool Disassembler::onWaitKeyPressed(uint16_t reg) {return false;}
bool Disassembler::onSetDelayTimer(uint16_t reg) {return false;}
bool Disassembler::onSetSoundTimer(uint16_t reg) {return false;}
bool Disassembler::onAddVxToI(uint16_t reg) {return false;}
bool Disassembler::onSetIToSpriteLoc(uint16_t reg) {return false;}
bool Disassembler::onStoreBCDOfVxInI(uint16_t reg) {return false;}
bool Disassembler::onStoreVnInI(uint16_t reg) {return false;}
bool Disassembler::onReadVnFromI(uint16_t reg) {return false;}
