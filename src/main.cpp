//
//  main.cpp
//  chip8EMU
//
//  Created by Manuel Deneu on 24/03/2025.
//

#include "Emulator.h"
#include "Peripherals.hpp"
#include "Rom.hpp"
#include "SDLPeripherals.hpp"
#include <iostream>

bool checkFlag(int argc, const char *argv[], const char *flag) {
    for (int i = 0; i < argc; i++) {
        if (strcmp(argv[i], flag) == 0) {
            return true;
        }
    }
    return false;
}

bool getFlagValue(int argc, const char *argv[], const char *flag,
                  std::string &val) {
    for (int i = 0; i < argc; i++) {
        if (strcmp(argv[i], flag) == 0) {
            if (i + 1 < argc) {
                val = std::string(argv[i + 1]);
                return true;
            }
            return false;
        }
    }
    return true;
}

void printUsage() {
    printf("usage: inputfile [-h] [-v] \n");
    printf("-h: this help\n");
    printf("-v: verbose\n");
}

int main(int argc, const char *argv[]) {

    if (checkFlag(argc, argv, "-h")) {
        printUsage();
        return 0;
    }
    if (argc < 2 || argv[1][0] == '-') {
        printf("Error: no input file\n");
        printUsage();
        return 0;
    }
    bool verbose = false;
    const std::string inputFile = argv[1];
    if (checkFlag(argc, argv, "-v")) {
        verbose = true;
    }

    SDLPeripherals p;
    p.init();
    Rom rom;
    rom.loadFile(inputFile);

    Chip8::CPU emu({.logs = verbose});
    emu.init(&rom, &p);
    emu.run();
    return 0;
}
