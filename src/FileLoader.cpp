//
//  FileLoader.cpp
//  chip8EMU
//
//  Created by Manuel Deneu on 25/03/2025.
//

#include "FileLoader.hpp"
#include <fstream>
#include <stdint.h>
#include <vector>

bool Chip8::loadFile(const std::string &path, Chip8::Bytes &bytes) {
    std::ifstream file(path, std::ios::binary);

    if (!file.good()) {
        return false;
    }
    file.seekg(0, std::ios::end);
    std::streampos fileSize = file.tellg();
    file.seekg(0, std::ios::beg);

    // read the data:
    bytes.resize(fileSize);
    file.read((char *)&bytes[0], fileSize);
    file.close();
    return true;
}
