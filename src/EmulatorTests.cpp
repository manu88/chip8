//
//  EmulatorTests.cpp
//  chip8EMU
//
//  Created by Manuel Deneu on 24/04/2025.
//

#include "EmulatorTests.hpp"
#include "Emulator.h"
#include "Peripherals.hpp"
#include "Rom.hpp"
#include <assert.h>

struct StubPerih : Chip8::Peripherals {
    void resetStubState() { clearDisplayCalled = false; }
    void clearDisplay() override { clearDisplayCalled = true; }

    bool clearDisplayCalled = false;
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

void testADD1_noCarry() {
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

void testADD1_carry() {
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

void testADD2() {
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

void testADD3() {
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

void testOR() {
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

void testAND() {
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

void testXOR() {
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

void testSUB_1() {
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
    if (cpu.getRegisters().v[0X1] != (v1 - v2)) {
        assert(false);
    }
    if (cpu.getRegisters().v[0XF] != 1) {
        assert(false);
    }
}

void testSUB_2() {
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
    testADD1_noCarry();
    testADD1_carry();
    testADD2();
    testADD3();
    testOR();
    testAND();
    testXOR();
    testSUB_1();
    testSUB_2();
    return 0;
}

/*
 8xy6 - SHR Vx {, Vy}
 8xy7 - SUBN Vx, Vy
 8xyE - SHL Vx {, Vy}

 00EE - RET
 0nnn - SYS addr
 1nnn - JP addr
 Bnnn - JP V0, addr
 2nnn - CALL addrï

 9xy0 - SNE Vx, Vy
 4xkk - SNE Vx, byte
 5xy0 - SE Vx, Vy
 3xkk - SE Vx, byte

 // platform kinda dependant
 Dxyn - DRW Vx, Vy, nibble
 Ex9E - SKP Vx
 ExA1 - SKNP Vx
 Cxkk - RND Vx, byte
 Fx0A - LD Vx, K
 Fx29 - LD F, Vx
 Fx33 - LD B, Vx



 */
