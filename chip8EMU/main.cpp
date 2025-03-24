//
//  main.cpp
//  chip8EMU
//
//  Created by Manuel Deneu on 24/03/2025.
//

#include <iostream>
#include "emulator.h"
#include "Rom.hpp"


int main(int argc, const char * argv[]) {
    
    Rom rom;
    rom.loadFile("/Users/manueldeneu/Documents/dev/chip8EMU/roms/helloworld.rom");
    
    rom.dump();
    Chip8::CPU emu;
    emu.init(&rom);
    emu.run();

/*
    emu.exec(0x00E0);
    emu.exec(0x00EE);
    emu.exec(0x0ABC);
    emu.exec(0x1ABC);
    emu.exec(0x1EFE);
    emu.exec(0x2ABC);
    emu.exec(0x3ABC);
    emu.exec(0x4ABC);
    emu.exec(0x5AB0);
    emu.exec(0x6AB8);
    emu.exec(0x7AB8);
    emu.exec(0x8AB0);
    emu.exec(0x8AB1);
    emu.exec(0x8AB2);
    emu.exec(0x8AB3);
    emu.exec(0x8AB4);
    emu.exec(0x8AB5);
    emu.exec(0x8AB6);
    emu.exec(0x8AB7);
    emu.exec(0x8ABE);
    emu.exec(0x9AB0);
    emu.exec(0xABCD);
    emu.exec(0xBBCD);
    emu.exec(0xCBCD);
    emu.exec(0xD124);
    
    emu.exec(0xEA9E);
    emu.exec(0xEBA1);
    emu.exec(0xFA07);
    emu.exec(0xFA0A);
    emu.exec(0xFA15);
    emu.exec(0xFA18);
    emu.exec(0xFC1E);
    
    emu.exec(0xFC29);
    emu.exec(0xFC33);
    emu.exec(0xFC55);
    emu.exec(0xFC65);
    emu.exec(0xFC68);
 */
    return 0;
}
