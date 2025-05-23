//
//  Memory.cpp
//  chip8EMU
//
//  Created by Manuel Deneu on 24/03/2025.
//

#include "Memory.hpp"
#include "Rom.hpp"
#include <assert.h>

#define NUM_SPRITES 16
static const uint8_t Sprites[NUM_SPRITES][5] = {
    // 0
    {0b11110000, 0b10010000, 0b10010000, 0b10010000, 0b11110000},
    // 1
    {0b00100000, 0b01100000, 0b00100000, 0b00100000, 0b01110000},
    // 2
    {0b11110000, 0b00010000, 0b11110000, 0b10000000, 0b11110000},
    // 3
    {0b11110000, 0b00010000, 0b11110000, 0b00010000, 0b11110000},
    // 4
    {0b10010000, 0b10010000, 0b11110000, 0b00010000, 0b00010000},
    // 5
    {0b11110000, 0b10000000, 0b11110000, 0b00010000, 0b11110000},
    // 6
    {0b11110000, 0b10000000, 0b11110000, 0b10010000, 0b11110000},
    // 7
    {0b11110000, 0b00010000, 0b00100000, 0b01000000, 0b01000000},
    // 8
    {0b11110000, 0b10010000, 0b11110000, 0b10010000, 0b11110000},
    // 9
    {0b11110000, 0b10010000, 0b11110000, 0b00010000, 0b11110000},
    // A
    {0b11110000, 0b10010000, 0b11110000, 0b10010000, 0b10010000},
    // B
    {0b11100000, 0b10010000, 0b11100000, 0b10010000, 0b11100000},
    // C
    {0b11110000, 0b10000000, 0b10000000, 0b10000000, 0b11110000},
    // D
    {0b11100000, 0b10010000, 0b10010000, 0b10010000, 0b11100000},
    // E
    {0b11110000, 0b10000000, 0b11110000, 0b10000000, 0b11110000},
    // F
    {0b11110000, 0b10000000, 0b11110000, 0b10000000, 0b10000000},
};

void Chip8::Memory::setRom(Rom *rom) {
    _rom = rom;
    _rom->relocateDebugSymbols(ROM_ADDR);
}

void Chip8::Memory::reset() {
    _ram.clear();
    memset(stack, 0, STACK_SIZE * sizeof(uint16_t));
}

bool Chip8::Memory::isValid(uint16_t addr) const {
    if (addr >= ROM_ADDR) {
        uint16_t romAddr = addr - ROM_ADDR;
        return romAddr < _rom->bytes.size();
    }
    return false;
}

uint8_t Chip8::Memory::getValueAtAddr(uint16_t addr) const {
    if (addr >= ROM_ADDR && addr < ROM_ADDR + _rom->size()) {
        return _rom->bytes[addr - ROM_ADDR];
    }

    if (_ram.count(addr) == 0) {
        printf("Invalid memory address 0X%04X\n", addr);
        assert(false);
    }
    return _ram.at(addr);
}

bool Chip8::Memory::setValueAtAddr(uint16_t addr, uint16_t val) {
    // write in ROM region prohibited
    if (addr >= ROM_ADDR && addr < ROM_ADDR + _rom->size()) {
        return false;
    }
    _ram[addr] = val;
    return true;
}

uint8_t Chip8::Memory::getSpriteAddr(uint16_t val) const { return val; }

Chip8::Memory::Sprite Chip8::Memory::getSpriteData(uint16_t val) const {
    if (val >= NUM_SPRITES) {
        assert(false);
    }
    return {Sprites[val]};
}
