//
//  InstructionParser.hpp
//  chip8EMU
//
//  Created by Manuel Deneu on 26/03/2025.
//

#pragma once
#include "Config.hpp"
#include <stdint.h>

namespace Chip8 {
class InstructionParser {
  public:
    virtual ~InstructionParser() {}

    bool exec(uint16_t instruction);

  protected:
    InstructionParser(const Config &config) : _conf(config) {}

    virtual bool onNOP() { return false; }
    virtual bool onCLS() { return false; }                         // 00E0
    virtual bool onRET() { return false; }                         // 00EE
    virtual bool onCallMachine(uint16_t addr) { return false; }    // 0nnn
    virtual bool onJump(uint16_t addr) { return false; }           // 1nnn
    virtual bool onCallSubroutine(uint16_t addr) { return false; } // 2nnn
    virtual bool onSkipIfVxIsVal(uint16_t reg, uint16_t val) {
        return false;
    } // 3xkk
    virtual bool onSkipIfVxIsNotVal(uint16_t reg, uint16_t val) {
        return false;
    } // 4xkk
    virtual bool onSkipIfVxIsVy(uint16_t regX, uint16_t regY) {
        return false;
    } // 5xy0
    virtual bool onSetVx(uint16_t reg, uint16_t val) { return false; } // 6xkk
    virtual bool onAddValToVx(uint16_t reg, uint16_t val) {
        return false;
    } // 7xkk
    virtual bool onSetVxToVy(uint16_t regX, uint16_t regY) {
        return false;
    } // 8xy0
    virtual bool onOrValToVx(uint16_t regX, uint16_t regY) {
        return false;
    } // 8xy1
    virtual bool onAndValToVx(uint16_t reg, uint16_t val) {
        return false;
    } // 8xy2
    virtual bool onXOrValToVx(uint16_t reg, uint16_t val) {
        return false;
    } // 8xy3
    virtual bool onAddVyToVx(uint16_t regX, uint16_t regY) {
        return false;
    } // 8xy4
    virtual bool onSubVyToVx(uint16_t regX, uint16_t regY) {
        return false;
    } // 8xy5
    virtual bool onShiftRightVx(uint16_t reg) { return false; } // 8xy6
    virtual bool onSubVxToVy(uint16_t regX, uint16_t regY) {
        return false;
    } // 8xy7
    virtual bool onShiftLeftVx(uint16_t reg) { return false; } // 8xyE
    virtual bool onSkipNextIfVxIsNotVy(uint16_t regX, uint16_t regY) {
        return false;
    } // 9xy0
    virtual bool onSetI(uint16_t addr) { return false; }              // Annn
    virtual bool onJumpToLoc(uint16_t val) { return false; }          // Bnnn
    virtual bool onRand(uint16_t reg, uint16_t val) { return false; } // Cxkk
    virtual bool onDisplay(uint16_t regX, uint16_t regY, uint8_t nimble) {
        return false;
    } // Dxyn
    virtual bool onSkipIfKeyPressed(uint16_t reg) { return false; }    // Ex9E
    virtual bool onSkipIfKeyNotPressed(uint16_t reg) { return false; } // ExA1
    virtual bool onSetVxToDelayTimer(uint16_t reg) { return false; }   // Fx07
    virtual bool onWaitKeyPressed(uint16_t reg) { return false; }      // Fx0A
    virtual bool onSetDelayTimer(uint16_t reg) { return false; }       // Fx15
    virtual bool onSetSoundTimer(uint16_t reg) { return false; }       // Fx18
    virtual bool onAddVxToI(uint16_t reg) { return false; }            // Fx1E
    virtual bool onSetIToSpriteLoc(uint16_t reg) { return false; }     // Fx29
    virtual bool onStoreBCDOfVxInI(uint16_t reg) { return false; }     // Fx33
    virtual bool onStoreVnInI(uint16_t reg) { return false; }          // Fx55
    virtual bool onReadVnFromI(uint16_t reg) { return false; }         // Fx65

    // superChip instructions
    virtual bool onExit() { return false; }                           // 00FD
    virtual bool onSCR() { return false; }                            // 00FB
    virtual bool onSCL() { return false; }                            // 00FC
    virtual bool onScrollDown(uint8_t n) { return false; }            // 00Cn
    virtual bool onLowRes() { return false; }                         // 00FE
    virtual bool onHighRes() { return false; }                        // 00FF
    virtual bool onSetIToBigSpriteLoc(uint16_t reg) { return false; } // Fx30
    virtual bool onSaveFlagRegister(uint8_t reg) { return false; }    // Fx75
    virtual bool onLoadFlagRegister(uint8_t reg) { return false; }    // Fx85
    virtual bool onSuperChipDisplay(uint16_t regX, uint16_t regY) {
        return false;
    } // DxY0

    uint16_t currentInstruction;
    const Config _conf;
};
} // namespace Chip8
