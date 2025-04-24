
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
    enum { Size = 16 };
    uint8_t v[Size];
    uint16_t i;

    uint16_t pc;
    uint8_t sp;

    uint16_t soundTimer = 0;
    uint16_t delayTimer = 0;

    Registers() { reset(); }
    void reset() {
        memset(v, 0, Size);
        i = 0;
        sp = 0;
        pc = ROM_ADDR;
        delayTimer = 0;
        soundTimer = 0;
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
    void runOnce();
    void dump();

    Registers &getRegisters() { return _registers; }
    Memory &getMemory() { return _mem; }

  private:
    void advancePC();

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

    const Config &_conf;
    bool execAt(uint16_t memLoc);
    void updateTimers(double totalDurationMS);

    Registers _registers;

    Memory _mem;
    Peripherals *_peripherals;

    std::chrono::time_point<std::chrono::system_clock> _startTime;

    std::random_device _randomDevice;
    std::mt19937 _rng;
    std::uniform_int_distribution<uint8_t> _uint8Distrib;
};
} // namespace Chip8
