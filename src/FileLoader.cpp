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

static bool readFile(const std::string &filename, Chip8::Bytes &bytes) {
    std::ifstream file(filename, std::ios::binary);

    if (!file.good()) {
        return false;
    }
    file.seekg(0, std::ios::end);
    std::streampos fileSize = file.tellg();
    file.seekg(0, std::ios::beg);

    // read the data:
    bytes.resize(fileSize / 2);
    file.read((char *)&bytes[0], fileSize);
    file.close();
    return true;
}

bool Chip8::loadFile(const std::string &path, Chip8::Bytes &bytes) {
    if (!readFile(path, bytes)) {
        return false;
    }

    for (int i = 0; i < bytes.size(); i++) {
        bytes[i] = ((uint16_t)(bytes[i] >> 8)) | ((uint16_t)(bytes[i] << 8));
    }
    return true;
}
