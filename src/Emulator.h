
//
//  emulator.h
//  chip8EMU
//
//  Created by Manuel Deneu on 24/03/2025.
//

#pragma one

#include "Config.h"
#include "InstructionParser.hpp"
#include "Memory.hpp"
#include <chrono>
#include <random>
#include <stdint.h>
#include <string.h>

class Rom;

namespace Chip8 {

class Peripherals;

struct Registers {
    enum {Size = 16};
    uint16_t v[Size];
    uint16_t i;

    uint16_t pc;
    uint8_t sp;

    Registers() { reset(); }
    void reset() {
        memset(v, 0, Size);
        i = 0;
        sp = 0;
        pc = ROM_ADDR;
    }
};

class CPU : public InstructionParser {
  public:
    enum { CYCLE_MS = 16 }; // approx. 60Hz
    enum { DELAY_TIMER_HZ = 60 };

    CPU() : CPU(Config{}) {}
    CPU(const Config &config) : _conf(config), _rng(_randomDevice()) {}

    void init(Rom *rom, Peripherals *peripherals);
    void reset();
    void run();
    void dump();

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

    const Config &_conf;
    bool execAt(uint16_t memLoc);
    void updateTimers(double totalDurationMS);

    Registers _registers;

    Memory _mem;
    Peripherals *_peripherals;

    uint16_t _soundTimer = 0;
    uint16_t _delayTimer = 0;

    std::chrono::time_point<std::chrono::system_clock> _startTime;

    std::random_device _randomDevice;
    std::mt19937 _rng;
    std::uniform_int_distribution<uint8_t> _uint8Distrib;
};
} // namespace Chip8
