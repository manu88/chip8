//
//  Tests.cpp
//  asm
//
//  Created by Manuel Deneu on 26/03/2025.
//

#include "Tests.hpp"
#include "Assembler.hpp"
#include "Disassembler.hpp"
#include <assert.h>

static void TestInstruction(const std::string &inCode)
{
    Assembler a(inCode);
    auto b = a.generate();
    Disassembler d(b);
    d.params.noComments = true;
    auto code = d.generate();
    code.resize(code.size()-1); // remove last \n
    assert(code == inCode);
}

void RunTests() {
    TestInstruction("CLS");
    TestInstruction("RET");
    
    TestInstruction("LD V2, 0x78");
    TestInstruction("LD I, 0x500");
    TestInstruction("LD V3, 0x1");
    TestInstruction("LD V4, 0x1");
    TestInstruction("LD V1, K");
    TestInstruction("LD ST, V2");
    TestInstruction("LD F, V1");
    TestInstruction("DRW V3, V4, 0x5");
    TestInstruction("JP 0x200");
}
