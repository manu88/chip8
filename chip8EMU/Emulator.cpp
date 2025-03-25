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
    memset(_stack, 0, STACK_SIZE * sizeof(uint16_t));
    _soundTimer = 0;
    _startTime = std::chrono::system_clock::now();
    _rng.seed((unsigned int)_startTime.time_since_epoch().count());
    _uint8Distrib = std::uniform_int_distribution<uint8_t>();
}

void Chip8::CPU::updateTimers(double totalDurationMS) {
    if (_delayTimer > 0) {
        _delayTimer -= 1;
        if (_delayTimer == 0) {
            printf("Delay timer complete!\n");
        }
    }
}

void Chip8::CPU::run() {
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
        if (params.timeoutMS < 0) {
            params.timeoutMS = CYCLE_MS;
        }
        _peripherals->update(_mem, params);

        const std::chrono::duration<double, std::milli> completeCycle =
            std::chrono::system_clock::now() - before;
        updateTimers(completeCycle.count());
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
    for (int i = 0; i < STACK_SIZE; i++) {
        printf("0X%X: 0X%X %c\n", i, _stack[i], _registers.sp == i ? '*' : ' ');
    }
}

bool Chip8::CPU::execAt(uint16_t memLoc) {
    uint16_t instruction = _mem.getValueAtAddr(memLoc);
    return exec(instruction);
}

bool Chip8::CPU::exec(Instruction instruction) {
    if (_conf.logs) {
        // printf("Exec instruction 0X%0X at PC=0X%0X\n", instruction, _pc);
    }
    if (instruction == 0x00E0) {
        _peripherals->clearDisplay();
        _registers.pc += 1;
        return true;
    } else if (instruction == 0x00EE) {
        _registers.sp -= 1;
        _registers.pc = _stack[_registers.sp] + 1;
        return true;
    } else {
        const uint16_t opcode1 = (instruction & 0xF000) >> 12;
        if (opcode1 == 0) { // call 0NNN
            uint16_t addr = instruction & 0x0FFF;
            _registers.pc += 1;
            if (_conf.logs) {
                printf("[ignored] Call machine code at addr : 0x%x\n", addr);
            }
            return true;
        } else if (opcode1 == 1) { // 1NNN
            uint16_t addr = instruction & 0x0FFF;
            _registers.pc = addr;
            return true;
        } else if (opcode1 == 2) { // 2NNN
            uint16_t addr = instruction & 0x0FFF;
            _stack[_registers.sp] = _registers.pc;
            _registers.sp += 1;
            _registers.pc = addr;
            return true;
        } else if (opcode1 == 3) { // 3XNN
            uint16_t reg = (instruction & 0x0F00) >> 8;
            uint16_t val = instruction & 0x00FF;
            if (_registers.v[reg] == val) {
                _registers.pc += 1;
            }
            _registers.pc += 1;
            return true;
        } else if (opcode1 == 4) { // 4XNN
            uint16_t reg = (instruction & 0x0F00) >> 8;
            uint16_t val = instruction & 0x00FF;
            if (_registers.v[reg] != val) {
                _registers.pc += 1;
            }
            _registers.pc += 1;
            return true;
        } else if (opcode1 == 5) { // 5XY0
            uint16_t reg1 = (instruction & 0x0F00) >> 8;
            uint16_t reg2 = (instruction & 0x00F0) >> 4;

            if (_registers.v[reg1] == _registers.v[reg2]) {
                _registers.pc += 1;
            }
            _registers.pc += 1;
            return true;
        } else if (opcode1 == 6) { // 6XNN
            uint16_t reg = (instruction & 0x0F00) >> 8;
            uint16_t val = instruction & 0x00FF;
            _registers.v[reg] = val;
            _registers.pc += 1;
            return true;
        } else if (opcode1 == 7) { // 7XNN
            uint16_t reg = (instruction & 0x0F00) >> 8;
            uint16_t val = instruction & 0x00FF;
            printf("[TODO] Add 0x%x to v%x \n", val, reg);
            return false;
        } else if (opcode1 == 8) { // 8XY#
            const uint16_t opcode2 = instruction & 0x000F;
            const uint16_t reg1 = (instruction & 0x0F00) >> 8;
            const uint16_t reg2 = (instruction & 0x00F0) >> 4;

            if (opcode2 == 0) {
                printf("[TODO] Set V%x to the value of V%x\n", reg1, reg2);
                return false;
            } else if (opcode2 == 1) {
                printf("[TODO] Set V%x to the value of V%x OR V%x\n", reg1,
                       reg1, reg2);
                return false;
            } else if (opcode2 == 2) {
                printf("[TODO] Set V%x to the value of V%x AND V%x\n", reg1,
                       reg1, reg2);
                return false;
            } else if (opcode2 == 3) {
                printf("[TODO] Set V%x to the value of V%x xOR V%x\n", reg1,
                       reg1, reg2);
                return false;
            } else if (opcode2 == 4) {
                printf("[TODO] Add V%x to V%x TODO: VF is set to 1 when "
                       "there's an "
                       "overflow, and to 0 when there is not\n",
                       reg2, reg1);
                return false;
            } else if (opcode2 == 5) {
                printf("[TODO] V%x is subtracted from V%x TODO: VF is set to 0 "
                       "when "
                       "there's an underflow, and 1 when there is not. (i.e. "
                       "VF set to 1 if VX >= VY and 0 if not)\n",
                       reg2, reg1);
                return false;
            } else if (opcode2 == 6) {
                printf("[TODO] Shifts V%x to the right by 1, then stores the "
                       "least "
                       "significant bit of V%x prior to the shift into VF.\n",
                       reg1, reg1);
                return false;
            } else if (opcode2 == 7) {
                printf("[TODO] Sets V%x to V%x minus V%x. VF is set to 0 when "
                       "there's "
                       "an underflow, and 1 when there is not\n",
                       reg1, reg2, reg1);
                return false;
            } else if (opcode2 == 0xE) {
                printf("[TODO] Shifts V%x to the left by 1, then sets VF to 1 "
                       "if the "
                       "most significant bit of V%x prior to that shift was "
                       "set, or to 0 if it was unset\n",
                       reg1, reg1);
                return false;
            }
        } else if (opcode1 == 9) { // 9XY0
            const uint16_t reg1 = (instruction & 0x0F00) >> 8;
            const uint16_t reg2 = (instruction & 0x00F0) >> 4;
            printf(
                "[TODO] Skips the next instruction if V%x does not equal V%x\n",
                reg1, reg2);
            return false;
        } else if (opcode1 == 0xA) { // ANNN
            const uint16_t addr = instruction & 0x0FFF;
            _registers.i = addr;
            _registers.pc += 1;
            return true;
        } else if (opcode1 == 0xB) { // BNNN
            const uint16_t addr = instruction & 0x0FFF;
            printf("[TODO] Jumps to the address 0X%x plus V0\n", addr);
            return false;
        } else if (opcode1 == 0xC) { // CXNN
            const uint16_t reg = (instruction & 0x0F00) >> 8;
            const uint16_t val = instruction & 0x00FF;
            uint8_t randomVal = _uint8Distrib(_rng);
            _registers.v[reg] = randomVal & val;
            _registers.pc += 1;
            return true;
        } else if (opcode1 == 0xD) { // DXYN
            const uint16_t reg1 = (instruction & 0x0F00) >> 8;
            const uint16_t reg2 = (instruction & 0x00F0) >> 4;
            const uint16_t val = instruction & 0x000F;
            _peripherals->draw(_registers.v[reg1], _registers.v[reg2], val,
                               _registers.i);
            _registers.pc += 1;
            return true;
        } else if ((instruction & 0xF0FF) == 0xE09E) {
            const uint16_t reg = (instruction & 0x0F00) >> 8;
            printf(
                "[TODO] Skips the next instruction if the key stored in V%x is "
                "pressed\n",
                reg);
            return false;
        } else if ((instruction & 0xF0FF) == 0xE0A1) {
            const uint16_t reg = (instruction & 0x0F00) >> 8;
            printf("[TODO] Skips the next instruction if the key stored in V%x "
                   "is NOT "
                   "pressed\n",
                   reg);
            return false;
        } else if ((instruction & 0xF0FF) == 0xF007) {
            const uint16_t reg = (instruction & 0x0F00) >> 8;
            _registers.v[reg] = _delayTimer;
            _registers.pc += 1;
            return true;
        } else if ((instruction & 0xF0FF) == 0xF00A) {
            const uint16_t reg = (instruction & 0x0F00) >> 8;
            printf("A key press is awaited, and then stored in V%x\n", reg);
            _registers.v[reg] = _peripherals->waitKeyPress();
            _registers.pc += 1;
            return true;
        } else if ((instruction & 0xF0FF) == 0xF015) {
            const uint16_t reg = (instruction & 0x0F00) >> 8;
            _delayTimer = _registers.v[reg];
            _registers.pc += 1;
            return true;
        } else if ((instruction & 0xF0FF) == 0xF018) {
            const uint16_t reg = (instruction & 0x0F00) >> 8;
            _soundTimer = _registers.v[reg];
            _registers.pc += 1;
            return true;
        } else if ((instruction & 0xF0FF) == 0xF01E) {
            const uint16_t reg = (instruction & 0x0F00) >> 8;
            printf("[TODO] adds V%x to I\n", reg);
            return false;
        } else if ((instruction & 0xF0FF) == 0xF029) {
            const uint16_t reg = (instruction & 0x0F00) >> 8;
            _registers.i = _mem.getSpriteAddr(_registers.v[reg]);
            _registers.pc += 1;
            return true;
        } else if ((instruction & 0xF0FF) == 0xF033) {
            const uint16_t reg = (instruction & 0x0F00) >> 8;
            uint32_t result =
                dec2bcd_r(_registers.v[reg]); // 0X 0000 0000 0000 0000
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
            _registers.pc += 1;
            return true;
        } else if ((instruction & 0xF0FF) == 0xF055) {
            const uint16_t reg = (instruction & 0x0F00) >> 8;
            for (int i = 0; i <= reg; i++) {
                _mem.setValueAtAddr(_registers.i + i, _registers.v[i]);
            }
            _registers.pc += 1;
            return true;
        } else if ((instruction & 0xF0FF) == 0xF065) {
            const uint16_t reg = (instruction & 0x0F00) >> 8;
            for (int i = 0; i <= reg; i++) {
                _registers.v[i] = _mem.getValueAtAddr(_registers.i + i);
            }
            _registers.pc += 1;
            return true;
        }
    }
    printf("Unknown instruction 0X%X\n", instruction);
    return false;
}
