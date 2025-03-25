//
//  RomFileReader.hpp
//  chip8EMU
//
//  Created by Manuel Deneu on 24/03/2025.
//

#pragma once
#include <stdint.h>
#include <string>
#include <vector>

class Rom {
  public:
    bool loadFile(const std::string &path);
    void dump();
    size_t size() const { return bytes.size(); }
    std::vector<uint16_t> bytes;
};
#include <stdio.h>
