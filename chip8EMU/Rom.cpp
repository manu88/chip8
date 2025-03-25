//
//  RomFileReader.cpp
//  chip8EMU
//
//  Created by Manuel Deneu on 24/03/2025.
//

#include "Rom.hpp"
#include <fstream>

static std::vector<uint16_t> readFile(const std::string &filename) {
    // open the file:
    std::ifstream file(filename, std::ios::binary);

    // get its size:
    file.seekg(0, std::ios::end);
    std::streampos fileSize = file.tellg();
    file.seekg(0, std::ios::beg);

    // read the data:
    std::vector<uint16_t> fileData(fileSize / 2);
    file.read((char *)&fileData[0], fileSize);
    return fileData;
}

bool Rom::loadFile(const std::string &path) {
    bytes = readFile(path);

    for (int i = 0; i < bytes.size(); i++) {
        bytes[i] = (bytes[i] >> 8) | (bytes[i] << 8);
    }
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
