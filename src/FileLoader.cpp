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

static Chip8::Bytes readFile(const std::string &filename) {
    std::ifstream file(filename, std::ios::binary);

    file.seekg(0, std::ios::end);
    std::streampos fileSize = file.tellg();
    file.seekg(0, std::ios::beg);

    // read the data:
    Chip8::Bytes fileData(fileSize / 2);
    file.read((char *)&fileData[0], fileSize);
    return fileData;
}

Chip8::Bytes Chip8::loadFile(const std::string &path) {
    Chip8::Bytes bytes = readFile(path);

    for (int i = 0; i < bytes.size(); i++) {
        bytes[i] = (bytes[i] >> 8) | (bytes[i] << 8);
    }
    return bytes;
}
