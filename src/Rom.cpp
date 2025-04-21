//
//  RomFileReader.cpp
//  chip8EMU
//
//  Created by Manuel Deneu on 24/03/2025.
//

#include "Rom.hpp"

bool Rom::loadFile(const std::string &path) {
    return Chip8::loadFile(path, bytes);
}

void Rom::dump() {
    int i = 0;
    printf(" rom size = %zi\n", bytes.size());
    for (uint8_t b : bytes) {
        if (i % 8 == 0)
            printf("\n 0X%0X: ", i);
        printf("0X%02X ", b);
        i++;
    }
    printf("\n");
}
