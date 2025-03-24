
//
//  emulator.h
//  chip8EMU
//
//  Created by Manuel Deneu on 24/03/2025.
//

#pragma one

#include <stdint.h>
#include <string.h>
#include "Memory.hpp"

class Rom;

namespace Chip8 {

class Peripherals;

struct Registers{
    uint16_t v[16];
    uint16_t i;
    
    Registers(){
        reset();
    }
    void reset(){
        memset(v, 0, 16);
        i = 0;
    }
};

typedef uint16_t Instruction;

enum OpCode {
    CALL_MASK = 0xF000,
    DISPLAY_CLEAR = 0x00E0,
    RETURN = 0x00EE,
    GOTO_MASK = 0xF000,
};

class CPU {
  public:
    void init(Rom *rom, Peripherals *peripherals);
    void reset();
    void run();
    bool exec(Instruction instruction);
    void dump();
  private:
    
    uint16_t getSpriteAddr(uint16_t val);
    bool execAt(uint16_t memLoc);
    
    Registers _registers;
    uint16_t _pc;
    
    Memory _mem;
    Peripherals *_peripherals;
    
    uint16_t _soundTimer = 0;
};
} // namespace Chip8
