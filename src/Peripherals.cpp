//
//  Peripherals.cpp
//  chip8EMU
//
//  Created by Manuel Deneu on 24/03/2025.
//

#include "Peripherals.hpp"
#include "Emulator.hpp"
#include "Memory.hpp"
#include <stdio.h>
#include <stdlib.h>

bool Chip8::Peripherals::init() {
    reset();
    return true;
}

void Chip8::Peripherals::reset() {
    memset(&buffer, 0,
           sizeof(uint8_t) * HIGH_RES_SCREEN_WIDTH * HIGH_RES_SCREEN_HEIGTH);
}

uint8_t Chip8::Peripherals::waitKeyPress() {
    printf("Wait for key press\n");
    int val = getchar();
    printf("Val=0X%X\n", val);
    return 4;
}

std::vector<uint8_t> Chip8::Peripherals::getKeysPressed() { return {}; }

void Chip8::Peripherals::clearDisplay() { printf("Display clear\n"); }

bool Chip8::Peripherals::renderSprite(
    const Chip8::Memory &memory, const Chip8::Peripherals::DrawCommand &cmd) {
    bool somethingWasErased = false;
    const auto sprite = memory.getSpriteData(cmd.i);
    for (int y = 0; y < cmd.height; y++) {
        uint8_t v = sprite.data[y];
        for (int x = 0; x < 8; x++) {
            int xP = _scrollXOffset + (cmd.x + 7 - x);
            if (xP < 0) {
                xP = _currentWidth - xP;
            }
            int yP = _scrollYOffset + (cmd.y + y);
            if (yP < 0) {
                yP = _currentHeight - yP;
            }
            xP %= _currentWidth;
            yP %= _currentHeight;
            uint8_t currentScreenVal = buffer[xP][yP];
            buffer[xP][yP] ^= v & 0x0001;
            if (currentScreenVal) {
                somethingWasErased = true;
            }
            v >>= 1;
        }
    }
    return somethingWasErased;
}

bool Chip8::Peripherals::update(
    CPU &cpu, const Chip8::Peripherals::UpdateParams &params) {

    bool somethingWasErased = false;
    for (const auto &cmd : _commands) {
        if (renderSprite(cpu.getMemory(), cmd)) {
            somethingWasErased = true;
        }
    }
    _commands.clear();
    return somethingWasErased;
}

void Chip8::Peripherals::draw(uint16_t x, uint16_t y, uint16_t height,
                              uint16_t i) {
    _commands.push_back({x, y, height, i});
}

/*static*/ uint8_t Chip8::Peripherals::getKeyCode(char key) {
    // hackish

    switch (std::tolower(key)) {
    case '0':
        return 0;
    case '1':
        return 1;
    case '2':
        return 2;
    case '3':
        return 3;
    case '4':
        return 4;
    case '5':
        return 5;
    case '6':
        return 6;
    case '7':
        return 7;
    case '8':
        return 8;
    case '9':
        return 9;
    case 'a':
        return 10;
    case 'b':
        return 11;
    case 'c':
        return 12;
    case 'd':
        return 13;
    case 'e':
        return 14;
    case 'f':
        return 15;
    default:
        break;
    }
    return 0;
}

void Chip8::Peripherals::changeMode(bool highRes) {
    _highRes = highRes;
    if (_highRes) {
        _currentWidth = HIGH_RES_SCREEN_WIDTH;
        _currentHeight = HIGH_RES_SCREEN_HEIGTH;
    } else {
        _currentWidth = LOW_RES_SCREEN_WIDTH;
        _currentHeight = LOW_RES_SCREEN_HEIGTH;
    }
}

uint16_t Chip8::Peripherals::getRand() { return arc4random_uniform(256); }

void Chip8::Peripherals::scroll(ScrollDirection direction, uint8_t amount) {
    switch (direction) {
    case Chip8::Peripherals::Down:
        _scrollYOffset += amount;
        break;
    case Chip8::Peripherals::Left:
        _scrollXOffset -= amount;
        break;
    case Chip8::Peripherals::Right:
        _scrollXOffset += amount;
        break;
    }
}
