//
//  Peripherals.cpp
//  chip8EMU
//
//  Created by Manuel Deneu on 24/03/2025.
//

#include "Peripherals.hpp"
#include <stdio.h>

uint8_t Chip8::Peripherals::waitKeyPress() {
    printf("Wait for key press\n");
    int val = getchar();
    printf("Val=0X%X\n", val);
    return 4;
}

void Chip8::Peripherals::clearDisplay() { printf("Display clear\n"); }

void Chip8::Peripherals::update(
    const Chip8::Memory &memory, const Chip8::Registers &registers,
    const Chip8::Peripherals::UpdateParams &params) {}

void Chip8::Peripherals::draw(uint16_t x, uint16_t y, uint16_t height,
                              uint16_t i) {
    printf("Draw at x=0X%0X, y=0X%0X height=0X%0X, i=0X%0X\n", x, y, height, i);
}

/*static*/ uint8_t Chip8::Peripherals::getKeyCode(char key) {
    // hackish
    if (key == '0') {
        return 0;
    }
    if (key == '1') {
        return 1;
    }
    if (key == '2') {
        return 2;
    }
    if (key == '3') {
        return 3;
    }
    if (key == '4') {
        return 4;
    }
    if (key == '5') {
        return 5;
    }
    if (key == '6') {
        return 6;
    }
    if (key == '7') {
        return 7;
    }
    if (key == '8') {
        return 8;
    }
    if (key == '9') {
        return 9;
    }
    if (std::tolower(key) == 'a') {
        return 10;
    }
    if (std::tolower(key) == 'b') {
        return 11;
    }
    if (std::tolower(key) == 'c') {
        return 12;
    }
    if (std::tolower(key) == 'd') {
        return 13;
    }
    if (std::tolower(key) == 'e') {
        return 14;
    }
    if (std::tolower(key) == 'f') {
        return 15;
    }
    return 0;
}
