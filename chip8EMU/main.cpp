//
//  main.cpp
//  chip8EMU
//
//  Created by Manuel Deneu on 24/03/2025.
//

#include "Peripherals.hpp"
#include "Rom.hpp"
#include "SDLPeripherals.hpp"
#include "emulator.h"
#include <iostream>

int main(int argc, const char *argv[]) {

    SDLPeripherals p;
    p.init();
    Rom rom;
    rom.loadFile(
        "/Users/manueldeneu/Documents/dev/chip8EMU/roms/rand_tester.rom");

    rom.dump();
    Chip8::CPU emu;
    emu.init(&rom, &p);
    emu.run();
    return 0;
}
