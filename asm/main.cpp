//
//  main.cpp
//  asm
//
//  Created by Manuel Deneu on 25/03/2025.
//

#include "Assembler.hpp"
#include "Disassembler.hpp"
#include "Tests.hpp"
#include <fstream>
#include <iostream>

int runAssembler(const std::string &binFilePath,
                 const std::string &outFilePath) {
    Assembler assembler;
    if (!assembler.loadFile(binFilePath)) {
        printf("unable to read from file '%s'\n", binFilePath.c_str());
        return 1;
    }
    auto binary = assembler.generate();
    if (binary.size() == 0) {
        return 1;
    }
    std::ofstream fs(outFilePath,
                     std::ios::out | std::ios::binary | std::ios::app);
    if (!fs.good()) {
        return 1;
    }
    for (const auto b : binary) {
        uint16_t byte = (b >> 8) | (b << 8);
        fs.write(reinterpret_cast<const char *>(&byte), 2);
    }
    fs.close();
    return 0;

    return 1;
}

int runDisassembler(const std::string &binFilePath,
                    const std::string &outFilePath) {
    Disassembler disassembler;
    if (!disassembler.loadFile(binFilePath)) {
        printf("unable to read from file '%s'\n", binFilePath.c_str());
        return 1;
    }
    std::string code = disassembler.generate();
    if (code.size() == 0) {
        return 1;
    }
    std::ofstream outFile;
    outFile.open(outFilePath);
    if (!outFile.good()) {
        printf("unable to open file '%s' for writting\n", outFilePath.c_str());
        return 1;
    }
    outFile << code;
    outFile.close();
    return 0;
}

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
    printf("usage: inputfile [-h] [-d] [-t] [-o ouput.file]\n");
    printf("-h: this help\n");
    printf("-d: use disassembler\n");
    printf("-t: run tests\n");
}

int main(int argc, const char *argv[]) {
    bool useAssembler = true;
    if (checkFlag(argc, argv, "-h")) {
        printUsage();
        return 0;
    }
    if (checkFlag(argc, argv, "-t")) {
        printf("run tests\n");
        RunTests();
        return 0;
    }
    if (argc < 2 || argv[1][0] == '-') {
        printf("Error: no input file\n");
        printUsage();
        return 0;
    }
    const std::string inputFile = argv[1];
    if (checkFlag(argc, argv, "-d")) {
        useAssembler = false;
    }

    std::string outputFile;
    if (checkFlag(argc, argv, "-o")) {
        if (!getFlagValue(argc, argv, "-o", outputFile)) {
            printf("argument to '-o' is missing\n");
            return 1;
        }
    }
    if (useAssembler) {
        if (outputFile == "") {
            outputFile = inputFile + ".bin";
        }
        return runAssembler(inputFile, outputFile);

    } else {
        if (outputFile == "") {
            outputFile = inputFile + ".asm";
        }
        return runDisassembler(inputFile, outputFile);

        return 0;
    }
}
