//
//  RomFileReader.cpp
//  chip8EMU
//
//  Created by Manuel Deneu on 24/03/2025.
//

#include "Rom.hpp"

bool Rom::loadFile(const std::string &path) {
    bytes = Chip8::loadFile(path);
    return true;
}

void Rom::dump() {
    int i = 0;
    for (uint16_t b : bytes) {
        if (i % 8 == 0)
            printf("\n 0X%0X: ", i);
        printf("0X%04X ", b);
        i++;
    }
    printf("\n");
}
