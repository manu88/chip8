//
//  emulator.cpp
//  chip8EMU
//
//  Created by Manuel Deneu on 24/03/2025.
//

#include "Emulator.hpp"
#include "Peripherals.hpp"
#include "Rom.hpp"
#include <algorithm>
#include <assert.h>
#include <iostream>
#include <stdio.h>

static uint32_t dec2bcd_r(uint16_t dec) {
    return (dec) ? ((dec2bcd_r(dec / 10) << 4) + (dec % 10)) : 0;
}

void Chip8::CPU::init(Rom *rom, Peripherals *peripherals) {
    _mem.setRom(rom);
    _peripherals = peripherals;
    _peripherals->init();
    reset();
}

void Chip8::CPU::reset() {
    _registers.reset();
    _startTime = std::chrono::system_clock::now();
}

void Chip8::CPU::updateTimers(double totalDurationMS) {
    if (_registers.delayTimer > 0) {
        _registers.delayTimer -= 1;
        if (_registers.delayTimer == 0) {
        }
    }
}

void Chip8::CPU::advancePC() { _registers.pc += 2; }

void Chip8::CPU::runOnce() {
    if (_conf.debugInstructions) {
        if (debugCtx.paused) {
            return;
        }
        if (debugCtx.stepNext) {
            debugCtx.stepNext = false;
        }
    }
    uint16_t pc = _registers.pc;
    if (!execAt(pc)) {
        if (_conf.logs) {
            printf("Unable to exec instruction 0X%0X at pc=0X%0X\n",
                   currentInstruction, pc);
        }
    }
}

void Chip8::CPU::run() {
    long frameId = 0;
    while (!_peripherals->shouldStop()) {
        const auto before = std::chrono::system_clock::now();
        runOnce();
        const std::chrono::duration<double, std::milli> cpuDuration =
            std::chrono::system_clock::now() - before;
        Chip8::Peripherals::UpdateParams params;
        params.timeoutMS = CYCLE_MS - cpuDuration.count();
        params.frameId = frameId;
        if (params.timeoutMS < 0) {
            params.timeoutMS = CYCLE_MS;
        }

        if (_peripherals->update(*this, params)) {
            _registers.v[0XF] = 1;
        }

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

bool Chip8::CPU::onNOP() {
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
    if (_conf.logs) {
        printf("[ignored] Call machine code at addr : 0x%x\n", addr);
    }
    advancePC();
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

bool Chip8::CPU::onSetVxToVy(uint16_t regX, uint16_t regY) {
    // Stores the value of register Vy in register Vx.
    _registers.v[regX] = _registers.v[regY];
    advancePC();
    return true;
}

bool Chip8::CPU::onOrValToVx(uint16_t regX, uint16_t regY) {
    // 8xy1 - OR Vx, Vy
    _registers.v[regX] |= _registers.v[regY];
    advancePC();
    return true;
}

bool Chip8::CPU::onAndValToVx(uint16_t regX, uint16_t regY) {
    // Set Vx = Vx AND Vy.
    _registers.v[regX] &= _registers.v[regY];
    advancePC();
    return true;
}

bool Chip8::CPU::onXOrValToVx(uint16_t regX, uint16_t regY) {
    // Set Vx = Vx XOR Vy.
    _registers.v[regX] ^= _registers.v[regY];
    advancePC();
    return true;
}

bool Chip8::CPU::onAddVyToVx(uint16_t regX, uint16_t regY) {
    // Set Vx = Vx + Vy, set VF = carry.
    uint16_t result = _registers.v[regX] + _registers.v[regY];
    _registers.v[regX] = (uint8_t)result;
    _registers.v[0xF] = result >= 0XFF ? 1 : 0;
    advancePC();
    return true;
}

bool Chip8::CPU::onSubVyToVx(uint16_t regX, uint16_t regY) {
    // Set Vx = Vx - Vy, set VF = NOT borrow.
    //  If Vx > Vy, then VF is set to 1, otherwise 0. Then Vy is subtracted from
    //  Vx, and the results stored in Vx.
    _registers.v[0xF] = _registers.v[regX] > _registers.v[regY] ? 1 : 0;
    _registers.v[regX] = (uint8_t)(_registers.v[regX] - _registers.v[regY]);
    advancePC();
    return true;
}

bool Chip8::CPU::onShiftRightVx(uint16_t reg) {
    // Set Vx = Vx SHR 1.
    // If the least-significant bit of Vx is 1, then VF is set to 1, otherwise
    // 0. Then Vx is divided by 2.
    _registers.v[0xF] = _registers.v[reg] & 1;
    _registers.v[reg] >>= 1;
    advancePC();
    return true;
}

bool Chip8::CPU::onSubVxToVy(uint16_t regX, uint16_t regY) {
    // Set Vx = Vy - Vx, set VF = NOT borrow.
    // If Vy > Vx, then VF is set to 1, otherwise 0. Then Vx is subtracted from
    // Vy, and the results stored in Vx.
    _registers.v[0xF] = _registers.v[regY] > _registers.v[regX] ? 1 : 0;
    _registers.v[regX] = (uint8_t)(_registers.v[regY] - _registers.v[regX]);
    advancePC();
    return true;
}

bool Chip8::CPU::onShiftLeftVx(uint16_t reg) {
    // Set Vx = Vx SHL 1.
    _registers.v[0xF] = _registers.v[reg] & 1;
    _registers.v[reg] <<= 1;
    advancePC();
    advancePC();
    return true;
}

bool Chip8::CPU::onSkipNextIfVxIsNotVy(uint16_t regX, uint16_t regY) {
    if (_registers.v[regX] != _registers.v[regY]) {
        advancePC();
    }
    advancePC();
    return true;
}

bool Chip8::CPU::onSetI(uint16_t addr) {
    _registers.i = addr;
    advancePC();
    return true;
}

bool Chip8::CPU::onJumpToLoc(uint16_t val) {
    // Jump to location nnn + V0.
    uint16_t addr = val + _registers.v[0];
    _registers.pc = addr;
    return true;
}

bool Chip8::CPU::onRand(uint16_t reg, uint16_t val) {
    uint8_t randomVal = _peripherals->getRand();
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

bool Chip8::CPU::onWaitKeyPressed(uint16_t reg) {
    _registers.v[reg] = _peripherals->waitKeyPress();
    advancePC();
    return true;
}

bool Chip8::CPU::onSkipIfKeyPressed(uint16_t reg) {
    // Skip next instruction if key with the value of Vx is pressed.
    auto keys = _peripherals->getKeysPressed();
    if (std::find(keys.begin(), keys.end(), _registers.v[reg]) != keys.end()) {
        advancePC();
    }
    advancePC();
    return true;
}

bool Chip8::CPU::onSkipIfKeyNotPressed(uint16_t reg) {
    auto keys = _peripherals->getKeysPressed();
    if (std::find(keys.begin(), keys.end(), _registers.v[reg]) == keys.end()) {
        advancePC();
    }
    advancePC();
    return true;
}

bool Chip8::CPU::onSetVxToDelayTimer(uint16_t reg) {
    _registers.v[reg] = _registers.delayTimer;
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

bool Chip8::CPU::onAddVxToI(uint16_t reg) {
    // Set I = I + Vx.
    _registers.i += _registers.v[reg];
    advancePC();
    return true;
}

bool Chip8::CPU::onSetIToSpriteLoc(uint16_t reg) {
    _registers.i = _mem.getSpriteAddr(_registers.v[reg]);
    advancePC();
    return true;
}

bool Chip8::CPU::onSetIToBigSpriteLoc(uint16_t reg) {
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
        printf("Error writing 0X%04X at 0X%04X\n", d2, _registers.i);
        return false;
    }
    if (!_mem.setValueAtAddr(_registers.i + 1, d1)) {
        printf("Error writing 0X%04X at 0X%04X\n", d1, _registers.i + 1);
        return false;
    }
    if (!_mem.setValueAtAddr(_registers.i + 2, d0)) {
        printf("Error writing 0X%04X at 0X%04X\n", d0, _registers.i + 2);
        return false;
    }
    advancePC();
    return true;
}

bool Chip8::CPU::onStoreVnInI(uint16_t reg) {
    for (int i = 0; i <= reg; i++) {
        if (!_mem.setValueAtAddr(_registers.i + i, _registers.v[i])) {
            return false;
        }
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

bool Chip8::CPU::onExit() {
    _peripherals->signalExit();
    advancePC();
    return true;
}

bool Chip8::CPU::onSCR() {
    _peripherals->scroll(Peripherals::ScrollDirection::Right, 1);
    advancePC();
    return true;
}

bool Chip8::CPU::onSCL() {
    _peripherals->scroll(Peripherals::ScrollDirection::Left, 1);
    advancePC();
    return true;
}

bool Chip8::CPU::onScrollDown(uint8_t n) {
    _peripherals->scroll(Peripherals::ScrollDirection::Down, n);
    advancePC();
    return true;
}

bool Chip8::CPU::onLowRes() {
    _peripherals->changeMode(false);
    advancePC();
    return true;
}

bool Chip8::CPU::onHighRes() {
    _peripherals->changeMode(true);
    advancePC();
    return true;
}

bool Chip8::CPU::onSaveFlagRegister(uint8_t reg) {
    assert(false);
    return false;
}

bool Chip8::CPU::onLoadFlagRegister(uint8_t reg) {
    assert(false);
    return false;
}

bool Chip8::CPU::onSuperChipDisplay(uint16_t regX, uint16_t regY) {
    assert(false);
    return false;
}
