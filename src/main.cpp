//
//  main.cpp
//  chip8EMU
//
//  Created by Manuel Deneu on 24/03/2025.
//

#include "Assembler.hpp"
#include "Emulator.h"
#include "EmulatorTests.hpp"
#include "Peripherals.hpp"
#include "Rom.hpp"
#include "SDLPeripherals.hpp"
#include "TermPeripherals.hpp"
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
    printf("usage: inputfile [-h] [-v] [-a] [-g] [-t]\n");
    printf("-h: this help\n");
    printf("-v: verbose\n");
    printf("-a: compile input file\n");
    printf("-g: use GUI\n");
    printf("-t: run tests\n");
}

static Chip8::Peripherals *createPeripherals(bool useGui) {
    if (useGui) {
        return new SDLPeripherals();
    }
    return new TermPeripherals();
}

static bool buildAsm(const std::string &inputFile, Rom &rom) {
    Assembler assembler;
    if (!assembler.loadFile(inputFile)) {
        printf("unable to read from file '%s'\n", inputFile.c_str());
        return false;
    }
    rom.bytes = assembler.generate();

    if (assembler.getError().has_value()) {
        printf("%s:%i: error: %s\n", inputFile.c_str(),
               assembler.getError().value().line,
               assembler.getError().value().msg.c_str());
        return false;
    }
    return true;
}

int main(int argc, const char *argv[]) {

    if (checkFlag(argc, argv, "-h")) {
        printUsage();
        return 0;
    }
    if (checkFlag(argc, argv, "-t")) {
        printf("run tests\n");
        return runEmulatorTests();
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

    bool inputIsAsm = false;
    if (checkFlag(argc, argv, "-a")) {
        inputIsAsm = true;
    }

    bool useGUI = false;
    if (checkFlag(argc, argv, "-g")) {
        useGUI = true;
    }

    Rom rom;
    if (inputIsAsm) {
        if (!buildAsm(inputFile, rom)) {
            return 1;
        }
    } else {
        if (!rom.loadFile(inputFile)) {
            printf("unable to read from file '%s'\n", inputFile.c_str());
            return 1;
        }
    }
    auto p = createPeripherals(useGUI);
    p->init();

    Chip8::CPU emu({.logs = verbose});
    emu.init(&rom, p);
    emu.run();
    delete p;
    return 0;
}
