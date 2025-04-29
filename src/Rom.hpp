//
//  RomFileReader.hpp
//  chip8EMU
//
//  Created by Manuel Deneu on 24/03/2025.
//

#pragma once
#include "FileLoader.hpp"
#include <string>

class Rom {
  public:
    bool loadFile(const std::string &path);
    void dump();
    size_t size() const { return bytes.size(); }
    Chip8::Bytes bytes;
};
