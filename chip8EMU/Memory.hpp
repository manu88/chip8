//
//  Memory.hpp
//  chip8EMU
//
//  Created by Manuel Deneu on 24/03/2025.
//

#pragma once

#include <stdint.h>

#define ROM_ADDR (uint16_t) 0x200
#define SPRITE_ADDR (uint16_t) 0x100


class Rom;
namespace Chip8 {
class Memory {
  public:
    void setRom(Rom *rom);
    uint16_t getValueAtAddr(uint16_t addr);

  private:
    Rom *_rom;
};
} // namespace Chip8
