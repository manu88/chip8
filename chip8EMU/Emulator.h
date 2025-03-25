
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

class Rom;

namespace Chip8 {

class Peripherals;

struct Registers {
    uint16_t v[16];
    uint16_t i;

    Registers() { reset(); }
    void reset() {
        memset(v, 0, 16);
        i = 0;
    }
};

typedef uint16_t Instruction;

class CPU {
  public:
    enum { CYCLE_MS = 16 }; // approx. 60Hz
    enum { DELAY_TIMER_HZ = 60 };
    enum { STACK_SIZE = 16 };

    CPU() : CPU(Config{}) {}
    CPU(const Config &config) : _conf(config), _rng(_randomDevice()) {}

    void init(Rom *rom, Peripherals *peripherals);
    void reset();
    void run();
    bool exec(Instruction instruction);
    void dump();

  private:
    const Config &_conf;
    bool execAt(uint16_t memLoc);
    void updateTimers(double totalDurationMS);

    Registers _registers;
    uint16_t _pc;
    uint8_t _sp;
    uint16_t _stack[STACK_SIZE];

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
