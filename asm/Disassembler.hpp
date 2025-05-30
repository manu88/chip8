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
 Follows this assembly language:
 http://devernay.free.fr/hacks/chip8/C8TECH10.HTM#3.1
 */
class Disassembler : public Chip8::InstructionParser {
  public:
    Disassembler(const Chip8::Bytes &bytes,
                 Chip8::Config conf = Chip8::Config())
        : Chip8::InstructionParser(conf), _bytes(bytes) {}

    std::string generate();

    struct Parameters {
        bool noAutogeneratedComments = false;
        bool showOriginalInstructionAsComment = true;
    };
    Parameters params;

  private:
    bool onNOP() override;
    bool onCLS() override;
    bool onRET() override;
    bool onCallMachine(uint16_t addr) override;
    bool onJump(uint16_t addr) override;
    bool onCallSubroutine(uint16_t addr) override;
    bool onSkipIfVxIsVal(uint16_t reg, uint16_t val) override;
    bool onSkipIfVxIsNotVal(uint16_t reg, uint16_t val) override;
    bool onSkipIfVxIsVy(uint16_t regX, uint16_t regY) override;
    bool onSetVx(uint16_t reg, uint16_t val) override;
    bool onAddValToVx(uint16_t reg, uint16_t val) override;
    bool onSetVxToVy(uint16_t regX, uint16_t regY) override;
    bool onOrValToVx(uint16_t reg, uint16_t val) override;
    bool onAndValToVx(uint16_t reg, uint16_t val) override;
    bool onXOrValToVx(uint16_t reg, uint16_t val) override;
    bool onAddVyToVx(uint16_t regX, uint16_t regY) override;
    bool onSubVyToVx(uint16_t regX, uint16_t regY) override;
    bool onShiftRightVx(uint16_t reg) override;
    bool onSubVxToVy(uint16_t regX, uint16_t regY) override;
    bool onShiftLeftVx(uint16_t reg) override;
    bool onSkipNextIfVxIsNotVy(uint16_t regX, uint16_t regY) override;
    bool onSetI(uint16_t addr) override;
    bool onJumpToLoc(uint16_t val) override;
    bool onRand(uint16_t reg, uint16_t val) override;
    bool onDisplay(uint16_t regX, uint16_t regY, uint8_t nimble) override;
    bool onSkipIfKeyPressed(uint16_t reg) override;
    bool onSkipIfKeyNotPressed(uint16_t reg) override;
    bool onSetVxToDelayTimer(uint16_t reg) override;
    bool onWaitKeyPressed(uint16_t reg) override;
    bool onSetDelayTimer(uint16_t reg) override;
    bool onSetSoundTimer(uint16_t reg) override;
    bool onAddVxToI(uint16_t reg) override;
    bool onSetIToSpriteLoc(uint16_t reg) override;
    bool onStoreBCDOfVxInI(uint16_t reg) override;
    bool onStoreVnInI(uint16_t reg) override;
    bool onReadVnFromI(uint16_t reg) override;

    // super instructions
    bool onExit() override;
    bool onSCR() override;
    bool onSCL() override;
    bool onScrollDown(uint8_t n) override;
    bool onLowRes() override;
    bool onHighRes() override;
    bool onSetIToBigSpriteLoc(uint16_t reg) override;
    bool onSaveFlagRegister(uint8_t reg) override;
    bool onLoadFlagRegister(uint8_t reg) override;
    bool onSuperChipDisplay(uint16_t regX, uint16_t regY) override;

    Chip8::Bytes _bytes;
    std::string _text;
};
