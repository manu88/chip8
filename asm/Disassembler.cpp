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

void Disassembler::onCLS() { _text += "CLS"; }
void Disassembler::onRET() { _text += "CLS"; }
void Disassembler::onCallMachine(uint16_t addr) {}
void Disassembler::onJump(uint16_t addr) {}
void Disassembler::onCallSubroutine(uint16_t addr) {}
void Disassembler::onSkipIfVxIsVal(uint16_t reg, uint16_t val) {}
void Disassembler::onSkipIfVxIsNotVal(uint16_t reg, uint16_t val) {}
void Disassembler::onSkipIfVxIsVy(uint16_t regX, uint16_t regY) {}
void Disassembler::onSetVx(uint16_t reg, uint16_t val) {
    _text += "mov v" + std::to_string(reg) + ", " + hex(val);
}
void Disassembler::onAddValToVx(uint16_t reg, uint16_t val) {}
void Disassembler::onSetVxToVy(uint16_t regX, uint16_t regY) {}
void Disassembler::onOrValToVx(uint16_t reg, uint16_t val) {}
void Disassembler::onAndValToVx(uint16_t reg, uint16_t val) {}
void Disassembler::onXOrValToVx(uint16_t reg, uint16_t val) {}
void Disassembler::onAddVyToVx(uint16_t regX, uint16_t regY) {}
void Disassembler::onSubVyToVx(uint16_t regX, uint16_t regY) {}
void Disassembler::onShiftRightVx(uint16_t reg) {}
void Disassembler::onSubVxToVy(uint16_t regX, uint16_t regY) {}
void Disassembler::onShiftLeftVx(uint16_t reg) {}
void Disassembler::onSkipNextIfVxIsNotVy(uint16_t regX, uint16_t regY) {}
void Disassembler::onSetI(uint16_t addr) {}
void Disassembler::onJumpToLoc(uint16_t val) {}
void Disassembler::onRand(uint16_t reg, uint16_t val) {}
void Disassembler::onDisplay(uint16_t regX, uint16_t regY, uint8_t nimble) {}
void Disassembler::onSkipIfKeyPressed(uint16_t reg) {}
void Disassembler::onSkipIfKeyNotPressed(uint16_t reg) {}
void Disassembler::onSetVxToDelayTimer(uint16_t reg) {}
void Disassembler::onWaitKeyPressed(uint16_t reg) {}
void Disassembler::onSetDelayTimer(uint16_t reg) {}
void Disassembler::onSetSoundTimer(uint16_t reg) {}
void Disassembler::onAddVxToI(uint16_t reg) {}
void Disassembler::onSetIToSpriteLoc(uint16_t reg) {}
void Disassembler::onStoreBCDOfVxInI(uint16_t reg) {}
void Disassembler::onStoreVnInI(uint16_t reg) {}
void Disassembler::onReadVnFromI(uint16_t reg) {}
