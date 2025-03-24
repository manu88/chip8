//
//  Peripherals.hpp
//  chip8EMU
//
//  Created by Manuel Deneu on 24/03/2025.
//

#pragma once
#include <stdint.h>

namespace Chip8 {
class Peripherals{
public:
    void draw(uint16_t x, uint16_t y, uint16_t height, uint16_t i);
    uint8_t waitKeyPress();
    void clearDisplay();
};
}
