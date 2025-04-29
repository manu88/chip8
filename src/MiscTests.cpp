//
//  MistTests.cpp
//  chip8EMU
//
//  Created by Manuel Deneu on 29/04/2025.
//

#include "EmulatorTests.hpp"
#include "Peripherals.hpp"
#include <assert.h>

static void testKeyCode() {
    assert(Chip8::Peripherals::getKeyCode('0') == 0);
    assert(Chip8::Peripherals::getKeyCode('1') == 1);
    assert(Chip8::Peripherals::getKeyCode('2') == 2);
    assert(Chip8::Peripherals::getKeyCode('3') == 3);
    assert(Chip8::Peripherals::getKeyCode('4') == 4);
    assert(Chip8::Peripherals::getKeyCode('5') == 5);
    assert(Chip8::Peripherals::getKeyCode('6') == 6);
    assert(Chip8::Peripherals::getKeyCode('7') == 7);
    assert(Chip8::Peripherals::getKeyCode('8') == 8);
    assert(Chip8::Peripherals::getKeyCode('9') == 9);
    assert(Chip8::Peripherals::getKeyCode('A') == 10);
    assert(Chip8::Peripherals::getKeyCode('a') == 10);
    assert(Chip8::Peripherals::getKeyCode('B') == 11);
    assert(Chip8::Peripherals::getKeyCode('b') == 11);
    assert(Chip8::Peripherals::getKeyCode('C') == 12);
    assert(Chip8::Peripherals::getKeyCode('c') == 12);
    assert(Chip8::Peripherals::getKeyCode('D') == 13);
    assert(Chip8::Peripherals::getKeyCode('d') == 13);
    assert(Chip8::Peripherals::getKeyCode('E') == 14);
    assert(Chip8::Peripherals::getKeyCode('e') == 14);
    assert(Chip8::Peripherals::getKeyCode('F') == 15);
    assert(Chip8::Peripherals::getKeyCode('f') == 15);
}
void runMiscTests() { testKeyCode(); }
