//
//  RomFileReader.hpp
//  chip8EMU
//
//  Created by Manuel Deneu on 24/03/2025.
//

#pragma once
#include <string>
#include <stdint.h>
#include <vector>

class Rom{
    public:
    bool loadFile(const std::string &path);
    void dump();
    std::vector<uint16_t> bytes;
};
#include <stdio.h>

