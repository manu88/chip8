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

/*
 Follows this assembly language: http://devernay.free.fr/hacks/chip8/C8TECH10.HTM#3.1
 */
class Disassembler : public Chip8::InstructionParser {
  public:
    bool loadFile(const std::string &path);

    std::string generate();

  private:
    bool onCLS();
    bool onRET();
    bool onCallMachine(uint16_t addr);
    bool onJump(uint16_t addr);
    bool onCallSubroutine(uint16_t addr);
    bool onSkipIfVxIsVal(uint16_t reg, uint16_t val);
    bool onSkipIfVxIsNotVal(uint16_t reg, uint16_t val);
    bool onSkipIfVxIsVy(uint16_t regX, uint16_t regY);
    bool onSetVx(uint16_t reg, uint16_t val);
    bool onAddValToVx(uint16_t reg, uint16_t val);
    bool onSetVxToVy(uint16_t regX, uint16_t regY);
    bool onOrValToVx(uint16_t reg, uint16_t val);
    bool onAndValToVx(uint16_t reg, uint16_t val);
    bool onXOrValToVx(uint16_t reg, uint16_t val);
    bool onAddVyToVx(uint16_t regX, uint16_t regY);
    bool onSubVyToVx(uint16_t regX, uint16_t regY);
    bool onShiftRightVx(uint16_t reg);
    bool onSubVxToVy(uint16_t regX, uint16_t regY);
    bool onShiftLeftVx(uint16_t reg);
    bool onSkipNextIfVxIsNotVy(uint16_t regX, uint16_t regY);
    bool onSetI(uint16_t addr);
    bool onJumpToLoc(uint16_t val);
    bool onRand(uint16_t reg, uint16_t val);
    bool onDisplay(uint16_t regX, uint16_t regY, uint8_t nimble);
    bool onSkipIfKeyPressed(uint16_t reg);
    bool onSkipIfKeyNotPressed(uint16_t reg);
    bool onSetVxToDelayTimer(uint16_t reg);
    bool onWaitKeyPressed(uint16_t reg);
    bool onSetDelayTimer(uint16_t reg);
    bool onSetSoundTimer(uint16_t reg);
    bool onAddVxToI(uint16_t reg);
    bool onSetIToSpriteLoc(uint16_t reg);
    bool onStoreBCDOfVxInI(uint16_t reg);
    bool onStoreVnInI(uint16_t reg);
    bool onReadVnFromI(uint16_t reg);

    Chip8::Bytes _bytes;
    std::string _text;
};
