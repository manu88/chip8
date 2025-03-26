//
//  Disassembler.hpp
//  asm
//
//  Created by Manuel Deneu on 25/03/2025.
//

#pragma once
#include "FileLoader.hpp"
#include "InstructionParser.hpp"
#include <stdint.h>
#include <vector>

class Disassembler : public Chip8::InstructionParser {
  public:
    bool loadFile(const std::string &path);

    std::string generate();

  private:
    void onCLS();
    void onRET();
    void onCallMachine(uint16_t addr);
    void onJump(uint16_t addr);
    void onCallSubroutine(uint16_t addr);
    void onSkipIfVxIsVal(uint16_t reg, uint16_t val);
    void onSkipIfVxIsNotVal(uint16_t reg, uint16_t val);
    void onSkipIfVxIsVy(uint16_t regX, uint16_t regY);
    void onSetVx(uint16_t reg, uint16_t val);
    void onAddValToVx(uint16_t reg, uint16_t val);
    void onSetVxToVy(uint16_t regX, uint16_t regY);
    void onOrValToVx(uint16_t reg, uint16_t val);
    void onAndValToVx(uint16_t reg, uint16_t val);
    void onXOrValToVx(uint16_t reg, uint16_t val);
    void onAddVyToVx(uint16_t regX, uint16_t regY);
    void onSubVyToVx(uint16_t regX, uint16_t regY);
    void onShiftRightVx(uint16_t reg);
    void onSubVxToVy(uint16_t regX, uint16_t regY);
    void onShiftLeftVx(uint16_t reg);
    void onSkipNextIfVxIsNotVy(uint16_t regX, uint16_t regY);
    void onSetI(uint16_t addr);
    void onJumpToLoc(uint16_t val);
    void onRand(uint16_t reg, uint16_t val);
    void onDisplay(uint16_t regX, uint16_t regY, uint8_t nimble);
    void onSkipIfKeyPressed(uint16_t reg);
    void onSkipIfKeyNotPressed(uint16_t reg);
    void onSetVxToDelayTimer(uint16_t reg);
    void onWaitKeyPressed(uint16_t reg);
    void onSetDelayTimer(uint16_t reg);
    void onSetSoundTimer(uint16_t reg);
    void onAddVxToI(uint16_t reg);
    void onSetIToSpriteLoc(uint16_t reg);
    void onStoreBCDOfVxInI(uint16_t reg);
    void onStoreVnInI(uint16_t reg);
    void onReadVnFromI(uint16_t reg);

    Chip8::Bytes _bytes;
    std::string _text;
};
