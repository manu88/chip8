//
//  Memory.hpp
//  chip8EMU
//
//  Created by Manuel Deneu on 24/03/2025.
//

#pragma once

#include <map>
#include <stdint.h>
#define ROM_ADDR (uint16_t)0x200
// #define SPRITE_ADDR (uint16_t) 0x100

class Rom;
namespace Chip8 {
class Memory {
    struct Sprite {
        const uint8_t *data;
    };

  public:
    bool isValid(uint16_t addr) const;
    void setRom(Rom *rom);
    uint16_t getValueAtAddr(uint16_t addr) const;
    uint16_t getSpriteAddr(uint16_t val) const;

    bool setValueAtAddr(uint16_t addr, uint16_t val);

    Sprite getSpriteData(uint16_t val) const;

  private:
    Rom *_rom;
    std::map<uint16_t, uint16_t> _ram;
};
} // namespace Chip8
