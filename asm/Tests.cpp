//
//  Tests.cpp
//  asm
//
//  Created by Manuel Deneu on 26/03/2025.
//

#include "Tests.hpp"
#include "Assembler.hpp"
#include "Disassembler.hpp"
#include "HexHelpers.hpp"
#include <assert.h>

static void SuperChipInstruction(void);

static void TestValidInstruction(const std::string &inCode,
                                 bool superChip = false) {
    Assembler a(inCode, {.superInstructions = superChip});
    auto b = a.generate();
    assert(a.getError() == std::nullopt);
    assert(a.getDebugSymbols().size() == 0);
    assert(b.size() > 0);
    Disassembler d(b, {.superInstructions = superChip});
    d.params.noAutogeneratedComments = true;
    d.params.showOriginalInstructionAsComment = false;
    auto code = d.generate();
    code.resize(code.size() - 1); // remove last \n
    assert(code == inCode);
}

static void RunTests1() {
    TestValidInstruction("CLS");
    TestValidInstruction("RET");
    TestValidInstruction("LD V2, 0X78");
    TestValidInstruction("LD I, 0X500");
    TestValidInstruction("LD V3, 0X1");
    TestValidInstruction("LD VC, 0X1");
    TestValidInstruction("LD V4, 0X1");
    TestValidInstruction("LD V1, K");
    TestValidInstruction("LD VD, K");
    TestValidInstruction("LD ST, V2");
    TestValidInstruction("LD F, V1");
    TestValidInstruction("DRW V3, V4, 0X5");
    TestValidInstruction("JP 0X200");
    TestValidInstruction("JP V0, 0X200");
    TestValidInstruction("LD V1, 0XA");
    TestValidInstruction("LD VB, DT");
    TestValidInstruction("LD V4, DT");
    TestValidInstruction("LD V1, I");
    TestValidInstruction("LD VA, I");
    TestValidInstruction("SE VA, 0X12");
    TestValidInstruction("LD DT, V3");
    TestValidInstruction("SKNP V6");
    TestValidInstruction("SNE V2, VA");
    TestValidInstruction("SNE V2, 0X12");
    TestValidInstruction("ADD V0, 0X1");
    TestValidInstruction("ADD V0, VC");
    TestValidInstruction("ADD I, VC");
    TestValidInstruction("CALL 0X124");
    TestValidInstruction("SYS 0X124");
    TestValidInstruction("RND V1, 0X24");
    TestValidInstruction("LD B, V1");
}

static void TestInvalidASM(const std::string &code, bool superChip = false) {
    Assembler a(code, {.superInstructions = superChip});
    auto b = a.generate();
    assert(a.getDebugSymbols().size() == 0);
    assert(b.size() == 0);
    assert(a.getError() != std::nullopt);
    assert(a.getError().value().line == 0);
}

static void RunTests2() {
    TestInvalidASM("lol\n");
    TestInvalidASM("LD\n");
    TestInvalidASM("LD G, Vx");
    TestInvalidASM("LD V2\n");
    TestInvalidASM("LD V2, lol");
    TestInvalidASM("JP V0, addr");
    TestInvalidASM("LD I, lol");
    TestInvalidASM("LD G abcd");
    TestInvalidASM("LD ST, abcd");
    TestInvalidASM("LD ST, v15");
    TestInvalidASM("LD F, lol");
    TestInvalidASM("LD F, v-1");
    TestInvalidASM("LD Vx, Vy");
    TestInvalidASM("LD Vaa, Vff");
    TestInvalidASM("DRW foo, V67, foo");
    TestInvalidASM("JP bar");
    TestInvalidASM("DRW V3, VT, 0x5");

    // missing arguments
    TestInvalidASM("LD V2");
    TestInvalidASM("LD I, ");
    TestInvalidASM("LD V3");
    TestInvalidASM("LD VC");
    TestInvalidASM("LD V4");
    TestInvalidASM("LD V1");
    TestInvalidASM("LD VD");
    TestInvalidASM("LD ST");
    TestInvalidASM("LD F");
    TestInvalidASM("DRW V3, V4");
    TestInvalidASM("DRW V3, ");
    TestInvalidASM("JP ");
    TestInvalidASM("JP V0");
    TestInvalidASM("LD V1");
    TestInvalidASM("LD VB");
    TestInvalidASM("LD V4, LOL");
    TestInvalidASM("LD V1");
    TestInvalidASM("SE VA");
    TestInvalidASM("SE VA, foo");
    TestInvalidASM("LD DT");
    TestInvalidASM("LD DT, 0X12");
    TestInvalidASM("SKNP ");
    TestInvalidASM("SNE V2");
    TestInvalidASM("SNE V2");
    TestInvalidASM("ADD V0");
    TestInvalidASM("ADD V0");
    TestInvalidASM("ADD I");
    TestInvalidASM("ADD J");
    TestInvalidASM("CALL ");
    TestInvalidASM("SYS ");
    TestInvalidASM("RND ");
    TestInvalidASM("RND V0");
    TestInvalidASM("LD B, 0X12");
    TestInvalidASM("LD B");

    // n=0 is a super chip only
    TestInvalidASM("DRW V3, V4, 0");
}

static void RunTests3_ForwardLabel() {
    std::string inCode = "CALL print\nprint:\nLD I, 0X500";
    Assembler a(inCode);
    auto b = a.generate();
    assert(a.getError() == std::nullopt);
    assert(b.size() > 0);
}

static void AllInstructions() {
    TestValidInstruction("CLS");
    TestValidInstruction("RET");
    TestValidInstruction("SYS 0XFA1");
    TestValidInstruction("JP 0XFA1");
    TestValidInstruction("CALL 0XFA1");
    TestValidInstruction("SE VA, 0XA1");
    TestValidInstruction("SNE VB, 0XF1");
    TestValidInstruction("SE V0, V1");
    TestValidInstruction("LD VD, 0XA1");
    TestValidInstruction("ADD VA, 0XA1");
    TestValidInstruction("LD V1, V2");
    TestValidInstruction("OR V4, V3");
    TestValidInstruction("AND V5, V6");
    TestValidInstruction("XOR V8, V7");
    TestValidInstruction("ADD V9, VA");
    TestValidInstruction("SUB VC, VB");
    TestValidInstruction("SHR VD"); // {, Vy}");
    TestValidInstruction("SUBN V1, V2");
    TestValidInstruction("SHL V3"); //{, Vy}");
    TestValidInstruction("SNE V4, VB");
    TestValidInstruction("LD I, 0XFA1");
    TestValidInstruction("JP V0, 0XFA1");
    TestValidInstruction("RND V1, 0XF1");
    TestValidInstruction("DRW V2, V3, 0XA");
    TestValidInstruction("SKP VA");
    TestValidInstruction("SKNP VB");
    TestValidInstruction("LD VC, DT");
    TestValidInstruction("LD VD, 0XFF");
    TestValidInstruction("LD DT, VA");
    TestValidInstruction("LD ST, VB");
    TestValidInstruction("ADD I, VC");
    TestValidInstruction("LD F, VD");
    TestValidInstruction("LD B, VE");
    TestValidInstruction("LD I, VA");
    TestValidInstruction("LD B, VA");
    TestValidInstruction("LD VA, I");
}

static void TestHexHelpers() {
    for (int i = 0; i < 16; i++) {
        bool valid = false;
        uint8_t r = parseRegisterAddr("V" + hex(i, false), valid);
        assert(valid);
        assert(r == i);
    }
}

static void TestsDebugInstructions() {
    std::string inCode = ";comment\nCLS\nLD V0, 0X0\nLD F, V0;this comment is "
                         "important\nLD V3, 0X1";
    Assembler a(inCode, {.debugInstructions = true});
    auto b = a.generate();
    assert(a.getError() == std::nullopt);
    assert(b.size() == 8);
    assert(a.getDebugSymbols().size() ==
           4); // we ditched the 1st line with comment
    auto line = a.getDebugSymbols().at(0X0004);
    assert(line == "LD F, V0;this comment is important");
}

void RunTests() {
    TestHexHelpers();
    RunTests1();
    RunTests2();
    RunTests3_ForwardLabel();
    AllInstructions();
    TestsDebugInstructions();
    SuperChipInstruction();
}

static void SuperChipInstruction() {
    TestValidInstruction("EXIT", true);
    TestValidInstruction("SCR", true);
    TestValidInstruction("SCL", true);
    TestValidInstruction("LOW", true);
    TestValidInstruction("HIGH", true);

    TestInvalidASM("SCD");
    TestInvalidASM("SCD, 1,2");
    for (int i = 0; i < 16; i++) {
        TestValidInstruction("SCD " + hex(i), true);
    }
    for (int i = 0; i < 16; i++) {
        TestValidInstruction("LD HF, V" + hex(i, false), true);
    }
    for (int i = 0; i < 16; i++) {
        if (i <= 7) {
            TestValidInstruction("LD R, V" + hex(i, false), true);
        } else {
            TestInvalidASM("LD R, V" + hex(i, false), true);
        }
    }
    for (int i = 0; i < 16; i++) {
        if (i <= 7) {
            TestValidInstruction("LD V" + hex(i, false) + ", R", true);
        } else {
            TestInvalidASM("LD V" + hex(i, false) + ", R", true);
        }
    }
    TestValidInstruction("DRW VA, VB, 0", true);
}
