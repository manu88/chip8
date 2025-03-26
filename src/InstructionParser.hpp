//
//  InstructionParser.hpp
//  chip8EMU
//
//  Created by Manuel Deneu on 26/03/2025.
//

#pragma once
#include <stdint.h>

namespace Chip8 {
class InstructionParser {
  public:
    virtual ~InstructionParser() {}

    bool exec(uint16_t instruction);

  protected:
    virtual void onCLS() {}                                             // 00E0
    virtual void onRET() {}                                             // 00EE
    virtual void onCallMachine(uint16_t addr) {}                        // 0nnn
    virtual void onJump(uint16_t addr) {}                               // 1nnn
    virtual void onCallSubroutine(uint16_t addr) {}                     // 2nnn
    virtual void onSkipIfVxIsVal(uint16_t reg, uint16_t val) {}         // 3xkk
    virtual void onSkipIfVxIsNotVal(uint16_t reg, uint16_t val) {}      // 4xkk
    virtual void onSkipIfVxIsVy(uint16_t regX, uint16_t regY) {}        // 5xy0
    virtual void onSetVx(uint16_t reg, uint16_t val) {}                 // 6xkk
    virtual void onAddValToVx(uint16_t reg, uint16_t val) {}            // 7xkk
    virtual void onSetVxToVy(uint16_t regX, uint16_t regY) {}           // 8xy0
    virtual void onOrValToVx(uint16_t reg, uint16_t val) {}             // 8xy1
    virtual void onAndValToVx(uint16_t reg, uint16_t val) {}            // 8xy2
    virtual void onXOrValToVx(uint16_t reg, uint16_t val) {}            // 8xy3
    virtual void onAddVyToVx(uint16_t regX, uint16_t regY) {}           // 8xy4
    virtual void onSubVyToVx(uint16_t regX, uint16_t regY) {}           // 8xy5
    virtual void onShiftRightVx(uint16_t reg) {}                        // 8xy6
    virtual void onSubVxToVy(uint16_t regX, uint16_t regY) {}           // 8xy7
    virtual void onShiftLeftVx(uint16_t reg) {}                         // 8xyE
    virtual void onSkipNextIfVxIsNotVy(uint16_t regX, uint16_t regY) {} // 9xy0
    virtual void onSetI(uint16_t addr) {}                                // Annn
    virtual void onJumpToLoc(uint16_t val) {}            // Bnnn
    virtual void onRand(uint16_t reg, uint16_t val) {}                 // Cxkk
    virtual void onDisplay(uint16_t regX, uint16_t regY, uint8_t nimble) {
    } // Dxyn
    virtual void onSkipIfKeyPressed(uint16_t reg) {}// Ex9E
    virtual void onSkipIfKeyNotPressed(uint16_t reg){} // ExA1
    virtual void onSetVxToDelayTimer(uint16_t reg){} // Fx07
    virtual void onWaitKeyPressed(uint16_t reg){} // Fx0A
    virtual void onSetDelayTimer(uint16_t reg){} // Fx15
    virtual void onSetSoundTimer(uint16_t reg){} // Fx18
    virtual void onAddVxToI(uint16_t reg){} // Fx1E
    virtual void onSetIToSpriteLoc(uint16_t reg){} // Fx29
    virtual void onStoreBCDOfVxInI(uint16_t reg){} // Fx33
    virtual void onStoreVnInI(uint16_t reg){} // Fx55
    virtual void onReadVnFromI(uint16_t reg){} // Fx65
    
};
} // namespace Chip8
