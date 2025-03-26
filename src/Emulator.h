
//
//  emulator.h
//  chip8EMU
//
//  Created by Manuel Deneu on 24/03/2025.
//

#pragma one

#include "Config.h"
#include "Memory.hpp"
#include <chrono>
#include <random>
#include <stdint.h>
#include <string.h>
#include "InstructionParser.hpp"

class Rom;

namespace Chip8 {

class Peripherals;

struct Registers {
    uint16_t v[16];
    uint16_t i;

    uint16_t pc;
    uint8_t sp;

    Registers() { reset(); }
    void reset() {
        memset(v, 0, 16);
        i = 0;
        sp = 0;
        pc = ROM_ADDR;
    }
};

class CPU: public InstructionParser{
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
