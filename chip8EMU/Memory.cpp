//
//  Memory.cpp
//  chip8EMU
//
//  Created by Manuel Deneu on 24/03/2025.
//

#include "Memory.hpp"
#include "Rom.hpp"
#include <assert.h>


void Chip8::Memory::setRom(Rom *rom){
    _rom = rom;
}

uint16_t Chip8::Memory::getValueAtAddr(uint16_t addr){
    if (addr >= ROM_ADDR){
        return _rom->bytes[addr - ROM_ADDR];
    }
    printf("Invalid memory address 0X%0X\n", addr);
    assert(false);
    return 0;
}
