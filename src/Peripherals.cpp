//
//  Peripherals.cpp
//  chip8EMU
//
//  Created by Manuel Deneu on 24/03/2025.
//

#include "Peripherals.hpp"
#include "Memory.hpp"
#include <stdio.h>

bool Chip8::Peripherals::init() {
    auto startTime = std::chrono::system_clock::now();
    _rng.seed((unsigned int)startTime.time_since_epoch().count());
    _uint8Distrib = std::uniform_int_distribution<uint8_t>();

    memset(&buffer, 0,
           sizeof(uint8_t) * HIGH_RES_SCREEN_WIDTH * HIGH_RES_SCREEN_HEIGTH);
    return true;
}

uint8_t Chip8::Peripherals::waitKeyPress() {
    printf("Wait for key press\n");
    int val = getchar();
    printf("Val=0X%X\n", val);
    return 4;
}

std::vector<uint8_t> Chip8::Peripherals::getKeysPressed() { return {}; }

void Chip8::Peripherals::clearDisplay() { printf("Display clear\n"); }

void Chip8::Peripherals::renderSprite(
    const Chip8::Memory &memory, const Chip8::Peripherals::DrawCommand &cmd) {
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

            buffer[xP % _currentWidth][yP % _currentHeight] = v & 0x0001;
            v >>= 1;
        }
    }
}

void Chip8::Peripherals::update(
    const Chip8::Memory &memory, const Chip8::Registers &registers,
    const Chip8::Peripherals::UpdateParams &params) {

    for (const auto &cmd : _commands) {
        renderSprite(memory, cmd);
    }
}

void Chip8::Peripherals::draw(uint16_t x, uint16_t y, uint16_t height,
                              uint16_t i) {
    _commands.push_back({x, y, height, i});
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

uint16_t Chip8::Peripherals::getRand() { return _uint8Distrib(_rng); }

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
