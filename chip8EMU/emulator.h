
//
//  emulator.h
//  chip8EMU
//
//  Created by Manuel Deneu on 24/03/2025.
//

#pragma one

#include <stdint.h>
#include <string.h>

class Rom;

namespace Chip8 {

struct Registers{
    uint16_t v[16];
    
    Registers(){
        reset();
    }
    void reset(){
        memset(v, 0, 16);
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
    void init(Rom *rom);
    void run();
    bool exec(Instruction instruction);
  private:
    bool execAt(uint16_t memLoc);
    
    Registers _registers;
    uint16_t _instructionPtr;
    
    Rom *_rom;
};
} // namespace Chip8
