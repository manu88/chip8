//
//  emulator.cpp
//  chip8EMU
//
//  Created by Manuel Deneu on 24/03/2025.
//

#include "Emulator.h"
#include "Peripherals.hpp"
#include "Rom.hpp"
#include <assert.h>
#include <iostream>
#include <stdio.h>

static uint32_t dec2bcd_r(uint16_t dec) {
    return (dec) ? ((dec2bcd_r(dec / 10) << 4) + (dec % 10)) : 0;
}

void Chip8::CPU::init(Rom *rom, Peripherals *peripherals) {
    _mem.setRom(rom);
    _peripherals = peripherals;
    reset();
}

void Chip8::CPU::reset() {
    _registers.reset();
    _startTime = std::chrono::system_clock::now();
    _rng.seed((unsigned int)_startTime.time_since_epoch().count());
    _uint8Distrib = std::uniform_int_distribution<uint8_t>();
}

void Chip8::CPU::updateTimers(double totalDurationMS) {
    if (_registers.delayTimer > 0) {
        _registers.delayTimer -= 1;
        if (_registers.delayTimer == 0) {
        }
    }
}

void Chip8::CPU::advancePC() { _registers.pc += 2; }

void Chip8::CPU::run() {
    long frameId = 0;
    while (!_peripherals->shouldStop()) {
        const auto before = std::chrono::system_clock::now();
        uint16_t pc = _registers.pc;
        if (!execAt(pc)) {
            if (_conf.logs) {
                printf("Unable to exec instruction at pc=0X%0X\n", pc);
            }
            return;
        }

        const std::chrono::duration<double, std::milli> cpuDuration =
            std::chrono::system_clock::now() - before;
        Chip8::Peripherals::UpdateParams params;
        params.timeoutMS = CYCLE_MS - cpuDuration.count();
        params.frameId = frameId;
        if (params.timeoutMS < 0) {
            params.timeoutMS = CYCLE_MS;
        }
        _peripherals->update(_mem, _registers, params);

        const std::chrono::duration<double, std::milli> completeCycle =
            std::chrono::system_clock::now() - before;
        updateTimers(completeCycle.count());
        frameId++;
    }
}

void Chip8::CPU::dump() {
    printf("registers:\n");
    for (int i = 0; i < 16; i++) {
        printf("V%x=0X%0X\n", i, _registers.v[i]);
    }
    printf("I=0X%0x\n", _registers.i);
    printf("pc=0X%0x\n", _registers.pc);
    printf("Stack ptr %i\n", _registers.sp);
    for (int i = 0; i < Chip8::Memory::STACK_SIZE; i++) {
        printf("0X%X: 0X%X %c\n", i, _mem.stack[i],
               _registers.sp == i ? '*' : ' ');
    }
}

bool Chip8::CPU::execAt(uint16_t memLoc) {
    uint8_t b0 = _mem.getValueAtAddr(memLoc);
    uint8_t b1 = _mem.getValueAtAddr(memLoc + 1);
    uint16_t instruction = (b0 << 8) + b1;
    return exec(instruction);
}

bool Chip8::CPU::onNOP(){
    advancePC();
    return true;
}

bool Chip8::CPU::onCLS() {
    _peripherals->clearDisplay();
    advancePC();
    return true;
}

bool Chip8::CPU::onRET() {
    assert(_registers.sp >= 1);
    _registers.sp -= 1;
    _registers.pc = _mem.stack[_registers.sp];
    advancePC();
    return true;
}

bool Chip8::CPU::onCallMachine(uint16_t addr) {
    advancePC();
    if (_conf.logs) {
        printf("[ignored] Call machine code at addr : 0x%x\n", addr);
    }
    return true;
}

bool Chip8::CPU::onJump(uint16_t addr) {
    _registers.pc = addr;
    return true;
}

bool Chip8::CPU::onCallSubroutine(uint16_t addr) {
    _mem.stack[_registers.sp] = _registers.pc;
    _registers.sp += 1;
    _registers.pc = addr;
    return true;
}

bool Chip8::CPU::onSkipIfVxIsVal(uint16_t reg, uint16_t val) {
    if (_registers.v[reg] == val) {
        advancePC();
    }
    advancePC();
    return true;
}

bool Chip8::CPU::onSkipIfVxIsNotVal(uint16_t reg, uint16_t val) {
    if (_registers.v[reg] != val) {
        advancePC();
    }
    advancePC();
    return true;
}

bool Chip8::CPU::onSkipIfVxIsVy(uint16_t regX, uint16_t regY) {
    if (_registers.v[regX] == _registers.v[regY]) {
        advancePC();
    }
    advancePC();
    return true;
}

bool Chip8::CPU::onSetVx(uint16_t reg, uint16_t val) {
    _registers.v[reg] = val;
    advancePC();
    return true;
}

bool Chip8::CPU::onAddValToVx(uint16_t reg, uint16_t val) {
    _registers.v[reg] += val;
    advancePC();
    return true;
}

bool Chip8::CPU::onSetVxToVy(uint16_t regX, uint16_t regY) { return false; }
bool Chip8::CPU::onOrValToVx(uint16_t regX, uint16_t regY) { return false; }
bool Chip8::CPU::onAndValToVx(uint16_t reg, uint16_t val) { return false; }
bool Chip8::CPU::onXOrValToVx(uint16_t reg, uint16_t val) { return false; }
bool Chip8::CPU::onAddVyToVx(uint16_t regX, uint16_t regY) { return false; }
bool Chip8::CPU::onSubVyToVx(uint16_t regX, uint16_t regY) { return false; }
bool Chip8::CPU::onShiftRightVx(uint16_t reg) { return false; }
bool Chip8::CPU::onSubVxToVy(uint16_t regX, uint16_t regY) { return false; }
bool Chip8::CPU::onShiftLeftVx(uint16_t reg) { return false; }

bool Chip8::CPU::onSkipNextIfVxIsNotVy(uint16_t regX, uint16_t regY) {
    return false;
}

bool Chip8::CPU::onSetI(uint16_t addr) {
    _registers.i = addr;
    advancePC();
    return true;
}

bool Chip8::CPU::onJumpToLoc(uint16_t val) { return false; }

bool Chip8::CPU::onRand(uint16_t reg, uint16_t val) {
    uint8_t randomVal = _uint8Distrib(_rng);
    _registers.v[reg] = randomVal & val;
    advancePC();
    return true;
}

bool Chip8::CPU::onDisplay(uint16_t regX, uint16_t regY, uint8_t nimble) {
    _peripherals->draw(_registers.v[regX], _registers.v[regY], nimble,
                       _registers.i);
    advancePC();
    return true;
}

bool Chip8::CPU::onSkipIfKeyPressed(uint16_t reg) { return false; }
bool Chip8::CPU::onSkipIfKeyNotPressed(uint16_t reg) { return false; }

bool Chip8::CPU::onSetVxToDelayTimer(uint16_t reg) {
    _registers.v[reg] = _registers.delayTimer;
    advancePC();
    return true;
}

bool Chip8::CPU::onWaitKeyPressed(uint16_t reg) {
    _registers.v[reg] = _peripherals->waitKeyPress();
    advancePC();
    return true;
}

bool Chip8::CPU::onSetDelayTimer(uint16_t reg) {
    _registers.delayTimer = _registers.v[reg];
    advancePC();
    return true;
}

bool Chip8::CPU::onSetSoundTimer(uint16_t reg) {
    _registers.soundTimer = _registers.v[reg];
    advancePC();
    return true;
}

bool Chip8::CPU::onAddVxToI(uint16_t reg) { return false; }

bool Chip8::CPU::onSetIToSpriteLoc(uint16_t reg) {
    _registers.i = _mem.getSpriteAddr(_registers.v[reg]);
    advancePC();
    return true;
}

bool Chip8::CPU::onStoreBCDOfVxInI(uint16_t reg) {
    uint32_t result = dec2bcd_r(_registers.v[reg]); // 0X 0000 0000 0000 0000
    const uint8_t d0 = result & 0x000F;
    const uint8_t d1 = (result & 0x00F0) >> 4;
    const uint8_t d2 = (result & 0x0F00) >> 8;
    if (!_mem.setValueAtAddr(_registers.i, d2)) {
        printf("Error writing 0X%0X at 0X%0X\n", d2, _registers.i);
    }
    if (!_mem.setValueAtAddr(_registers.i + 1, d1)) {
        printf("Error writing 0X%0X at 0X%0X\n", d1, _registers.i + 1);
    }
    if (!_mem.setValueAtAddr(_registers.i + 2, d0)) {
        printf("Error writing 0X%0X at 0X%0X\n", d0, _registers.i + 2);
    }
    advancePC();
    return true;
}

bool Chip8::CPU::onStoreVnInI(uint16_t reg) {
    for (int i = 0; i <= reg; i++) {
        _mem.setValueAtAddr(_registers.i + i, _registers.v[i]);
    }
    advancePC();
    return true;
}

bool Chip8::CPU::onReadVnFromI(uint16_t reg) {
    for (int i = 0; i <= reg; i++) {
        _registers.v[i] = _mem.getValueAtAddr(_registers.i + i);
    }
    advancePC();
    return true;
}
