//
//  EmulatorTests.cpp
//  chip8EMU
//
//  Created by Manuel Deneu on 24/04/2025.
//

#include "EmulatorTests.hpp"
#include "Emulator.hpp"
#include "Peripherals.hpp"
#include "Rom.hpp"
#include <assert.h>

struct StubPerih : Chip8::Peripherals {
    void resetStubState() { clearDisplayCalled = false; }
    void clearDisplay() override { clearDisplayCalled = true; }

    bool clearDisplayCalled = false;

    uint16_t getRand() override { return randVal; }
    uint16_t randVal = 42;

    uint8_t waitKeyPress() override { return waitKeyPressVal; }
    uint8_t waitKeyPressVal = 1;

    std::vector<uint8_t> getKeysPressed() override { return keysPressed; }
    std::vector<uint8_t> keysPressed;

    const std::vector<Chip8::Peripherals::DrawCommand> &commands() {
        return _commands;
    }
};

static void testCLS() {
    Chip8::CPU cpu;
    Rom r;
    StubPerih p;
    // clear 00E0 - CLS
    r.bytes.push_back(0x00);
    r.bytes.push_back(0xE0);

    cpu.init(&r, &p);
    cpu.runOnce();
    if (!p.clearDisplayCalled) {
        assert(false);
    }
    if (cpu.getRegisters().pc != ROM_ADDR + 2) {
        assert(false);
    }
}

static void testLD1() {
    Chip8::CPU cpu;
    Rom r;
    StubPerih p;
    // 6xkk - LD Vx, byte
    for (uint8_t i = 0; i < Chip8::Registers::Size; i++) {
        r.bytes.push_back(0x60 + i);
        r.bytes.push_back(i * 16);
    }

    cpu.init(&r, &p);
    for (uint8_t i = 0; i < Chip8::Registers::Size; i++) {
        cpu.runOnce();
        if (cpu.getRegisters().v[i] != i * 16) {
            assert(false);
        }
    }
}

static void testLD2() {
    // 8xy0 - LD Vx, Vy
    Chip8::CPU cpu;
    Rom r;
    StubPerih p;
    // LD V0, F1
    r.bytes.push_back(0x60);
    r.bytes.push_back(0xF1);
    for (uint8_t i = 1; i < Chip8::Registers::Size; i++) {
        // LD V1 = V0
        // LD V2 = V1
        // LD Vi, Vi-1
        r.bytes.push_back(0x80 + i);
        r.bytes.push_back((i - 1) * 0X10);
    }

    cpu.init(&r, &p);
    for (uint8_t i = 0; i < Chip8::Registers::Size; i++) {
        cpu.runOnce();
        if (cpu.getRegisters().v[i] != 0XF1) {
            assert(false);
        }
    }
}

static void testLD3() {
    // Annn - LD I, addr
    Chip8::CPU cpu;
    Rom r;
    StubPerih p;
    // 8xy0 - LD Vx, Vy
    r.bytes.push_back(0xAA);
    r.bytes.push_back(0xBC);

    cpu.init(&r, &p);
    cpu.runOnce();
    assert(cpu.getRegisters().i == 0XABC);
}

static void testLD4() {
    Chip8::CPU cpu;
    Rom r;
    StubPerih p;
    // Fx07 - LD Vx, DT
    r.bytes.push_back(0xFB);
    r.bytes.push_back(0x07);

    cpu.init(&r, &p);
    cpu.getRegisters().delayTimer = 0XCD;
    cpu.runOnce();
    assert(cpu.getRegisters().v[0XB] == 0XCD);
}

static void testLD5() {
    // Fx15 - LD DT, Vx
    Chip8::CPU cpu;
    Rom r;
    StubPerih p;
    // LD VC, 0XF1
    r.bytes.push_back(0x6C);
    r.bytes.push_back(0xF1);
    r.bytes.push_back(0xFC);
    r.bytes.push_back(0x15);

    cpu.init(&r, &p);
    cpu.runOnce();
    cpu.runOnce();
    assert(cpu.getRegisters().delayTimer == 0XF1);
}

static void testLD6() {
    // Fx18 - LD ST, Vx
    Chip8::CPU cpu;
    Rom r;
    StubPerih p;
    // LD VD, 0XFA
    r.bytes.push_back(0x6D);
    r.bytes.push_back(0xFA);
    r.bytes.push_back(0xFD);
    r.bytes.push_back(0x18);
    cpu.init(&r, &p);
    cpu.runOnce();
    cpu.runOnce();
    assert(cpu.getRegisters().soundTimer == 0XFA);
}

static void testLD7() {
    // Fx55 - LD [I], Vx
    Chip8::CPU cpu;
    Rom r;
    StubPerih p;
    cpu.init(&r, &p);
    for (int i = 0; i < 0X0E; i++) {
        cpu.getRegisters().v[i] = i;
    }
    cpu.getRegisters().i = 0X400;
    r.bytes.push_back(0xFE);
    r.bytes.push_back(0x55);
    cpu.runOnce();

    for (int i = 0; i < 0X0E; i++) {
        assert(cpu.getMemory().getValueAtAddr(0x400 + i) == i);
    }
}

static void testLD8() {
    // Fx65 - LD Vx, [I]
    Chip8::CPU cpu;
    Rom r;
    StubPerih p;
    cpu.init(&r, &p);
    for (int i = 0; i < Chip8::Registers::Size; i++) {
        cpu.getMemory().setValueAtAddr(0X400 + i, i);
    }
    cpu.getRegisters().i = 0x400;
    r.bytes.push_back(0xFF);
    r.bytes.push_back(0x65);
    cpu.runOnce();

    for (int i = 0; i < Chip8::Registers::Size; i++) {
        assert(cpu.getRegisters().v[i] == i);
    }
}

static void testLD9() {
    // Fx0A - LD Vx, K
    Chip8::CPU cpu;
    Rom r;
    StubPerih p;
    p.waitKeyPressVal = 0XA;
    cpu.init(&r, &p);
    r.bytes.push_back(0xFD);
    r.bytes.push_back(0x0A);
    cpu.runOnce();
    if (cpu.getRegisters().v[0XD] != p.waitKeyPressVal) {
        assert(false);
    }
}

static void testADD1_noCarry() {
    // 8xy4 - ADD Vx, Vy
    Chip8::CPU cpu;
    Rom r;
    StubPerih p;
    cpu.init(&r, &p);
    cpu.getRegisters().v[0XA] = 0X04;
    cpu.getRegisters().v[0XB] = 0X05;
    r.bytes.push_back(0x8A);
    r.bytes.push_back(0xB4);
    cpu.runOnce();
    if (cpu.getRegisters().v[0XA] != 0X09) {
        assert(false);
    }
    if (cpu.getRegisters().v[0XF] != 0) {
        assert(false);
    }
}

static void testADD1_carry() {
    // 8xy4 - ADD Vx, Vy
    Chip8::CPU cpu;
    Rom r;
    StubPerih p;
    cpu.init(&r, &p);
    cpu.getRegisters().v[0XA] = 0XFF;
    cpu.getRegisters().v[0XB] = 0X05;
    r.bytes.push_back(0x8A);
    r.bytes.push_back(0xB4);
    cpu.runOnce();
    if (cpu.getRegisters().v[0XA] != 0X04) {
        assert(false);
    }
    if (cpu.getRegisters().v[0XF] != 1) {
        assert(false);
    }
}

static void testADD2() {
    // 7xkk - ADD Vx, byte
    Chip8::CPU cpu;
    Rom r;
    StubPerih p;
    cpu.init(&r, &p);
    cpu.getRegisters().v[0XB] = 0X02;
    r.bytes.push_back(0x7B);
    r.bytes.push_back(0x10);
    cpu.runOnce();
    if (cpu.getRegisters().v[0XB] != 0X12) {
        assert(false);
    }
}

static void testADD3() {
    // Fx1E - ADD I, Vx
    Chip8::CPU cpu;
    Rom r;
    StubPerih p;
    cpu.init(&r, &p);
    uint8_t i = 0X0B;
    uint8_t vc = 0X02;
    cpu.getRegisters().i = i;
    cpu.getRegisters().v[0XC] = vc;
    r.bytes.push_back(0xFC);
    r.bytes.push_back(0x1E);
    cpu.runOnce();
    if (cpu.getRegisters().i != i + vc) {
        assert(false);
    }
}

static void testOR() {
    // 8xy1 - OR Vx, Vy
    Chip8::CPU cpu;
    Rom r;
    StubPerih p;
    cpu.init(&r, &p);
    uint8_t v1 = 0X0B;
    uint8_t v2 = 0X02;
    cpu.getRegisters().v[0XC] = v1;
    cpu.getRegisters().v[0XD] = v2;
    r.bytes.push_back(0x8C);
    r.bytes.push_back(0xD1);
    cpu.runOnce();
    if (cpu.getRegisters().v[0XC] != (v1 | v2)) {
        assert(false);
    }
}

static void testAND() {
    // 8xy2 - AND Vx, Vy
    Chip8::CPU cpu;
    Rom r;
    StubPerih p;
    cpu.init(&r, &p);
    uint8_t v1 = 0X0B;
    uint8_t v2 = 0XFF;
    cpu.getRegisters().v[0X1] = v1;
    cpu.getRegisters().v[0XE] = v2;
    r.bytes.push_back(0x81);
    r.bytes.push_back(0xE2);
    cpu.runOnce();
    if (cpu.getRegisters().v[0X1] != (v1 & v2)) {
        assert(false);
    }
}

static void testXOR() {
    // 8xy3 - XOR Vx, Vy
    Chip8::CPU cpu;
    Rom r;
    StubPerih p;
    cpu.init(&r, &p);
    uint8_t v1 = 0X0B;
    uint8_t v2 = 0XAA;
    cpu.getRegisters().v[0X1] = v1;
    cpu.getRegisters().v[0XE] = v2;
    r.bytes.push_back(0x81);
    r.bytes.push_back(0xE3);
    cpu.runOnce();
    if (cpu.getRegisters().v[0X1] != (v1 ^ v2)) {
        assert(false);
    }
}

static void testSUB_1() {
    // 8xy5 - SUB Vx, Vy
    Chip8::CPU cpu;
    Rom r;
    StubPerih p;
    cpu.init(&r, &p);
    uint8_t v1 = 0X0B;
    uint8_t v2 = 0X01;
    cpu.getRegisters().v[0X1] = v1;
    cpu.getRegisters().v[0XE] = v2;
    r.bytes.push_back(0x81);
    r.bytes.push_back(0xE5);
    cpu.runOnce();
    if (cpu.getRegisters().v[0X1] != v1 - v2) {
        assert(false);
    }
    if (cpu.getRegisters().v[0XF] != 1) {
        assert(false);
    }
}

static void testSUB_2() {
    // 8xy5 - SUB Vx, Vy
    Chip8::CPU cpu;
    Rom r;
    StubPerih p;
    cpu.init(&r, &p);
    uint8_t v1 = 0X01;
    uint8_t v2 = 0X0B;
    cpu.getRegisters().v[0X1] = v1;
    cpu.getRegisters().v[0XE] = v2;
    r.bytes.push_back(0x81);
    r.bytes.push_back(0xE5);
    cpu.runOnce();
    if (cpu.getRegisters().v[0X1] != uint8_t(v1 - v2)) {
        assert(false);
    }
    if (cpu.getRegisters().v[0XF] != 0) {
        assert(false);
    }
}

static void testSHR() {
    // 8xy6 - SHR Vx {, Vy}
    Chip8::CPU cpu;
    Rom r;
    StubPerih p;
    cpu.init(&r, &p);
    uint8_t v1 = 0X01;
    cpu.getRegisters().v[0XA] = v1;
    r.bytes.push_back(0x8A);
    r.bytes.push_back(0xB6);
    cpu.runOnce();
    uint8_t res = v1 >> 1;
    if (cpu.getRegisters().v[0XA] != res) {
        assert(false);
    }
}

static void testSHL() {
    // 8xyE - SHL Vx {, Vy}
    Chip8::CPU cpu;
    Rom r;
    StubPerih p;
    cpu.init(&r, &p);
    uint8_t v1 = 0X01;
    cpu.getRegisters().v[0XA] = v1;
    r.bytes.push_back(0x8A);
    r.bytes.push_back(0xBE);
    cpu.runOnce();
    uint8_t res = v1 << 1;
    if (cpu.getRegisters().v[0XA] != res) {
        assert(false);
    }
}

static void testSUBN_1() {
    // 8xy7 - SUBN Vx, Vy
    Chip8::CPU cpu;
    Rom r;
    StubPerih p;
    cpu.init(&r, &p);
    uint8_t v1 = 0X0B;
    uint8_t v2 = 0X01;
    cpu.getRegisters().v[0X1] = v1;
    cpu.getRegisters().v[0XE] = v2;
    r.bytes.push_back(0x81);
    r.bytes.push_back(0xE7);
    cpu.runOnce();
    uint8_t res = uint8_t(v2 - v1);
    if (cpu.getRegisters().v[0X1] != res) {
        assert(false);
    }
    if (cpu.getRegisters().v[0XF] != 0) {
        assert(false);
    }
}

static void testSUBN_2() {
    // 8xy7 - SUBN Vx, Vy
    Chip8::CPU cpu;
    Rom r;
    StubPerih p;
    cpu.init(&r, &p);
    uint8_t v1 = 0X01;
    uint8_t v2 = 0X0B;
    cpu.getRegisters().v[0X1] = v1;
    cpu.getRegisters().v[0XE] = v2;
    r.bytes.push_back(0x81);
    r.bytes.push_back(0xE7);
    cpu.runOnce();
    uint8_t res = uint8_t(v2 - v1);
    if (cpu.getRegisters().v[0X1] != res) {
        assert(false);
    }
    if (cpu.getRegisters().v[0XF] != 1) {
        assert(false);
    }
}

static void testRAND() {
    // Cxkk - RND Vx, byte
    Chip8::CPU cpu;
    Rom r;
    StubPerih p;
    p.randVal = 0X12;
    r.bytes.push_back(0xCA);
    r.bytes.push_back(0x12);
    cpu.init(&r, &p);
    cpu.runOnce();
    uint8_t res = p.randVal & 0X12;
    if (cpu.getRegisters().v[0XA] != res) {
        assert(false);
    }
}

static void testSNE1_1() {
    // 9xy0 - SNE Vx, Vy
    Chip8::CPU cpu;
    Rom r;
    StubPerih p;
    cpu.init(&r, &p);
    uint8_t v1 = 0X01;
    uint8_t v2 = 0X01;
    cpu.getRegisters().v[0X1] = v1;
    cpu.getRegisters().v[0XE] = v2;
    r.bytes.push_back(0x91);
    r.bytes.push_back(0xE0);
    auto addr = cpu.getRegisters().pc;
    cpu.runOnce();
    assert(cpu.getRegisters().pc == addr + 2);
}

static void testSNE1_2() {
    // 9xy0 - SNE Vx, Vy
    Chip8::CPU cpu;
    Rom r;
    StubPerih p;
    cpu.init(&r, &p);
    uint8_t v1 = 0X01;
    uint8_t v2 = 0X02;
    cpu.getRegisters().v[0X1] = v1;
    cpu.getRegisters().v[0XE] = v2;
    r.bytes.push_back(0x91);
    r.bytes.push_back(0xE0);
    auto addr = cpu.getRegisters().pc;
    cpu.runOnce();
    assert(cpu.getRegisters().pc == addr + 4);
}

static void testSNE2_1() {
    // 4xkk - SNE Vx, byte
    Chip8::CPU cpu;
    Rom r;
    StubPerih p;
    cpu.init(&r, &p);
    uint8_t v1 = 0X10;
    cpu.getRegisters().v[0XA] = v1;
    r.bytes.push_back(0x4A);
    r.bytes.push_back(0x10);
    auto addr = cpu.getRegisters().pc;
    cpu.runOnce();
    assert(cpu.getRegisters().pc == addr + 2);
}

static void testSNE2_2() {
    // 4xkk - SNE Vx, byte
    Chip8::CPU cpu;
    Rom r;
    StubPerih p;
    cpu.init(&r, &p);
    uint8_t v1 = 0X10;
    cpu.getRegisters().v[0XA] = v1;
    r.bytes.push_back(0x4A);
    r.bytes.push_back(0xAA);
    auto addr = cpu.getRegisters().pc;
    cpu.runOnce();
    assert(cpu.getRegisters().pc == addr + 4);
}

static void testSE1_1() {
    // 5xy0 - SE Vx, Vy
    Chip8::CPU cpu;
    Rom r;
    StubPerih p;
    cpu.init(&r, &p);
    uint8_t v1 = 0X01;
    uint8_t v2 = 0X01;
    cpu.getRegisters().v[0XA] = v1;
    cpu.getRegisters().v[0XB] = v2;
    r.bytes.push_back(0x5A);
    r.bytes.push_back(0xB0);
    auto addr = cpu.getRegisters().pc;
    cpu.runOnce();
    assert(cpu.getRegisters().pc == addr + 4);
}

static void testSE1_2() {
    // 5xy0 - SE Vx, Vy
    Chip8::CPU cpu;
    Rom r;
    StubPerih p;
    cpu.init(&r, &p);
    uint8_t v1 = 0X01;
    uint8_t v2 = 0XFF;
    cpu.getRegisters().v[0XA] = v1;
    cpu.getRegisters().v[0XB] = v2;
    r.bytes.push_back(0x5A);
    r.bytes.push_back(0xB0);
    auto addr = cpu.getRegisters().pc;
    cpu.runOnce();
    assert(cpu.getRegisters().pc == addr + 2);
}

static void testSE2_1() {
    // 3xkk - SE Vx, byte
    Chip8::CPU cpu;
    Rom r;
    StubPerih p;
    cpu.init(&r, &p);
    uint8_t v1 = 0XAB;
    cpu.getRegisters().v[0XA] = v1;
    r.bytes.push_back(0x3A);
    r.bytes.push_back(0xAB);
    auto addr = cpu.getRegisters().pc;
    cpu.runOnce();
    assert(cpu.getRegisters().pc == addr + 4);
}

static void testSE2_2() {
    // 3xkk - SE Vx, byte
    Chip8::CPU cpu;
    Rom r;
    StubPerih p;
    cpu.init(&r, &p);
    uint8_t v1 = 0XAB;
    cpu.getRegisters().v[0XA] = v1;
    r.bytes.push_back(0x3A);
    r.bytes.push_back(0xAC);
    auto addr = cpu.getRegisters().pc;
    cpu.runOnce();
    assert(cpu.getRegisters().pc == addr + 2);
}

static void testSKP_1() {
    // Ex9E - SKP Vx
    Chip8::CPU cpu;
    Rom r;
    StubPerih p;
    p.keysPressed = {0XA, 0X1, 0X2};
    cpu.init(&r, &p);
    cpu.getRegisters().v[0XD] = 0X1;
    r.bytes.push_back(0xED);
    r.bytes.push_back(0x9E);
    auto addr = cpu.getRegisters().pc;
    cpu.runOnce();
    assert(cpu.getRegisters().pc == addr + 4);
}

static void testSKP_2() {
    // Ex9E - SKP Vx
    Chip8::CPU cpu;
    Rom r;
    StubPerih p;
    p.keysPressed = {0XA, 0X1, 0X2};
    cpu.init(&r, &p);
    cpu.getRegisters().v[0XD] = 0XB;
    r.bytes.push_back(0xED);
    r.bytes.push_back(0x9E);
    auto addr = cpu.getRegisters().pc;
    cpu.runOnce();
    assert(cpu.getRegisters().pc == addr + 2);
}

static void testSKNP_1() {
    // ExA1 - SKNP Vx
    Chip8::CPU cpu;
    Rom r;
    StubPerih p;
    p.keysPressed = {0XA, 0X1, 0X2};
    cpu.init(&r, &p);
    cpu.getRegisters().v[0XD] = 0XB;
    r.bytes.push_back(0xED);
    r.bytes.push_back(0x9E);
    auto addr = cpu.getRegisters().pc;
    cpu.runOnce();
    assert(cpu.getRegisters().pc == addr + 2);
}

static void testSKNP_2() {
    // ExA1 - SKNP Vx
    Chip8::CPU cpu;
    Rom r;
    StubPerih p;
    p.keysPressed = {0XA, 0X1, 0X2};
    cpu.init(&r, &p);
    cpu.getRegisters().v[0XD] = 0X2;
    r.bytes.push_back(0xED);
    r.bytes.push_back(0x9E);
    auto addr = cpu.getRegisters().pc;
    cpu.runOnce();
    assert(cpu.getRegisters().pc == addr + 4);
}

static void testDRW() {
    // Dxyn - DRW Vx, Vy, nibble
    Chip8::CPU cpu;
    Rom r;
    StubPerih p;
    cpu.init(&r, &p);
    cpu.getRegisters().v[0XA] = 0X9;
    cpu.getRegisters().v[0XB] = 0XA;
    r.bytes.push_back(0xDA);
    r.bytes.push_back(0xB3);
    assert(p.commands().size() == 0);
    cpu.runOnce();
    assert(p.commands().size() == 1);
    auto cmd = p.commands().at(0);
    assert(cmd.x == 0X9);
    assert(cmd.y == 0XA);
    assert(cmd.height == 0X3);
}

static void testJP1() {
    // 1nnn - JP addr
    Chip8::CPU cpu;
    Rom r;
    StubPerih p;
    cpu.init(&r, &p);
    r.bytes.push_back(0x1A);
    r.bytes.push_back(0xBC);
    cpu.runOnce();
    assert(cpu.getRegisters().pc == 0XABC);
}

static void testJP2() {
    // Bnnn - JP V0, addr
    Chip8::CPU cpu;
    Rom r;
    StubPerih p;
    cpu.init(&r, &p);
    cpu.getRegisters().v[0] = 0XAB;
    r.bytes.push_back(0xB0);
    r.bytes.push_back(0xEF);
    cpu.runOnce();
    assert(cpu.getRegisters().pc == 0XAB + 0X0EF);
}

static void testCALL_RET() {
    // 2nnn - CALL addr
    // 00EE - RET
    Chip8::CPU cpu;
    Rom r;
    StubPerih p;
    cpu.init(&r, &p);
    r.bytes.push_back(0x22); // 0
    r.bytes.push_back(0x04); // 2006; CALL 0X204 (ROM ADDR = 0X200) + 4

    r.bytes.push_back(0x63); // 2
    r.bytes.push_back(0x42); // 6242;  LD V3, 0X42

    r.bytes.push_back(0x62); // 4
    r.bytes.push_back(0x42); // 6242;  LD V2, 0X42

    r.bytes.push_back(0x00); // 6
    r.bytes.push_back(0xEE); // 00EE; RET

    cpu.runOnce();
    // called
    assert(cpu.getRegisters().sp == 1);
    assert(cpu.getMemory().stack[0] == 0X200);

    cpu.runOnce();
    assert(cpu.getRegisters().v[0X2] == 0X42);
    // inside 'function‘

    cpu.runOnce();
    // ret
    assert(cpu.getRegisters().sp == 0);
    assert(cpu.getRegisters().pc == 0X202);

    assert(cpu.getRegisters().v[0X3] != 0X42);
}

static void testSYS() {
    // 0nnn - SYS addr
    // IMPORTANT: This instruction is *ignored* by this interpreter, pc is just
    // incremented.
    Chip8::CPU cpu;
    Rom r;
    StubPerih p;
    cpu.init(&r, &p);
    r.bytes.push_back(0x0A);
    r.bytes.push_back(0xBC);
    cpu.runOnce();
    assert(cpu.getRegisters().pc == 0X202);
}

static void testLD10() {
    //  Fx29 - LD F, Vx
    Chip8::CPU cpu;
    Rom r;
    StubPerih p;
    cpu.init(&r, &p);
    cpu.getRegisters().v[0XA] = 0XB;
    r.bytes.push_back(0xFA);
    r.bytes.push_back(0x29);
    cpu.runOnce();
    assert(cpu.getRegisters().i == 0XB);
}

static void testLD11() {
    //  Fx33 - LD B, Vx
    for (int val = 0; val < 256; val++) {
        Chip8::CPU cpu;
        Rom r;
        StubPerih p;
        cpu.init(&r, &p);
        cpu.getRegisters().v[0XA] = val;
        r.bytes.push_back(0xFA);
        r.bytes.push_back(0x33);
        cpu.runOnce();
        uint8_t bcd0 = cpu.getMemory().getValueAtAddr(cpu.getRegisters().i);
        uint8_t bcd1 = cpu.getMemory().getValueAtAddr(cpu.getRegisters().i + 1);
        uint8_t bcd2 = cpu.getMemory().getValueAtAddr(cpu.getRegisters().i + 2);
        uint8_t d100 = val / 100;
        assert(bcd0 == d100);
        uint8_t d10 = (val - d100 * 100) / 10;
        assert(bcd1 == d10);
        uint8_t d0 = (val - d100 * 100 - d10 * 10);
        assert(bcd2 == d0);
    }
}

static void testSuperChip(void);

int runEmulatorTests() {
    testCLS();
    testLD1();
    testLD2();
    testLD3();
    testLD4();
    testLD5();
    testLD6();
    testLD7();
    testLD8();
    testLD9();
    testLD10();
    testLD11();
    testADD1_noCarry();
    testADD1_carry();
    testADD2();
    testADD3();
    testOR();
    testAND();
    testXOR();
    testSUB_1();
    testSUB_2();
    testSHR();
    testSHL();
    testSUBN_1();
    testSUBN_2();
    testRAND();
    testSNE1_1();
    testSNE1_2();
    testSNE2_1();
    testSNE2_2();

    testSE1_1();
    testSE1_2();
    testSE2_1();
    testSE2_2();
    testSKP_1();
    testSKP_2();

    testSKNP_1();
    testSKNP_2();
    testDRW();
    testJP1();
    testJP2();
    testCALL_RET();
    testSYS();
    
    testSuperChip();
    return 0;
}

static void testSuperChip(){
    
}
