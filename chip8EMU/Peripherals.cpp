//
//  Peripherals.cpp
//  chip8EMU
//
//  Created by Manuel Deneu on 24/03/2025.
//

#include "Peripherals.hpp"
#include <stdio.h>

uint8_t Chip8::Peripherals::waitKeyPress(){
    printf("Wait for key press\n");
    getchar();
    return  4;
}

void Chip8::Peripherals::clearDisplay(){
    printf("Display clear\n");
}

void Chip8::Peripherals::draw(uint16_t x, uint16_t y, uint16_t height, uint16_t i){
    printf("Draw at x=0X%0X, y=0X%0X height=0X%0X, i=0X%0X\n", x, y, height, i);
}
