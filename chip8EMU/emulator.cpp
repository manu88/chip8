//
//  emulator.cpp
//  chip8EMU
//
//  Created by Manuel Deneu on 24/03/2025.
//

#include "emulator.h"
#include "Rom.hpp"
#include <stdio.h>

#define CHECK_BIT(var, pos) ((var) & (1 << (pos)))

void Chip8::CPU::init(Rom *rom) {
    _registers.reset();
    _instructionPtr = 0;
    _rom = rom;
}

void Chip8::CPU::run() {
    while (1){
        if (!execAt(_instructionPtr)){
            return;
        }
    }
}

bool Chip8::CPU::execAt(uint16_t memLoc) {
    uint16_t instruction = _rom->bytes[memLoc];
    printf("Exec instruction 0X%0X at location 0X%0X\n", instruction, memLoc);
    return exec(instruction);
}

bool Chip8::CPU::exec(Instruction instruction) {

    if (instruction == DISPLAY_CLEAR) {
        printf("Got display clear \n");
        return true;
    } else if (instruction == RETURN) {
        printf("Got return opcode\n");
        return false;
    } else {
        const uint16_t opcode1 = (instruction & 0xF000) >> 12;
        if (opcode1 == 0) { // call 0NNN
            uint16_t addr = instruction & 0x0FFF;
            printf("Call machine code at addr : 0x%x\n", addr);
            return false;
        } else if (opcode1 == 1) { // 1NNN
            uint16_t addr = instruction & 0x0FFF;
            printf("jump to addr : 0x%x\n", addr);
            return false;
        } else if (opcode1 == 2) { // 2NNN
            uint16_t addr = instruction & 0x0FFF;
            printf("call subroutine at addr : 0x%x\n", addr);
            return false;
        } else if (opcode1 == 3) { // 3XNN
            uint16_t reg = (instruction & 0x0F00) >> 8;
            uint16_t val = instruction & 0x00FF;
            printf("Skip next instruction if v%x equals 0x%x\n", reg, val);
            return false;
        } else if (opcode1 == 4) { // 4XNN
            uint16_t reg = (instruction & 0x0F00) >> 8;
            uint16_t val = instruction & 0x00FF;
            printf("Skip next instruction if v%x DOES NOT equal 0x%x\n", reg,
                   val);
            return false;
        } else if (opcode1 == 5) { // 5XY0
            uint16_t reg1 = (instruction & 0x0F00) >> 8;
            uint16_t reg2 = (instruction & 0x00F0) >> 4;
            printf("Skip next instruction if v%x equals v%x\n", reg1, reg2);
            return false;
        } else if (opcode1 == 6) { // 6XNN
            uint16_t reg = (instruction & 0x0F00) >> 8;
            uint16_t val = instruction & 0x00FF;
            _registers.v[reg] = val;
            _instructionPtr += 1;
            return true;
        } else if (opcode1 == 7) { // 7XNN
            uint16_t reg = (instruction & 0x0F00) >> 8;
            uint16_t val = instruction & 0x00FF;
            printf("Add 0x%x to v%x \n", val, reg);
            return false;
        } else if (opcode1 == 8) { // 8XY#
            const uint16_t opcode2 = instruction & 0x000F;
            const uint16_t reg1 = (instruction & 0x0F00) >> 8;
            const uint16_t reg2 = (instruction & 0x00F0) >> 4;

            if (opcode2 == 0) {
                printf("Set V%x to the value of V%x\n", reg1, reg2);
                return false;
            } else if (opcode2 == 1) {
                printf("Set V%x to the value of V%x OR V%x\n", reg1, reg1,
                       reg2);
                return false;
            } else if (opcode2 == 2) {
                printf("Set V%x to the value of V%x AND V%x\n", reg1, reg1,
                       reg2);
                return false;
            } else if (opcode2 == 3) {
                printf("Set V%x to the value of V%x xOR V%x\n", reg1, reg1,
                       reg2);
                return false;
            } else if (opcode2 == 4) {
                printf("Add V%x to V%x TODO: VF is set to 1 when there's an "
                       "overflow, and to 0 when there is not\n",
                       reg2, reg1);
                return false;
            } else if (opcode2 == 5) {
                printf("V%x is subtracted from V%x TODO: VF is set to 0 when "
                       "there's an underflow, and 1 when there is not. (i.e. "
                       "VF set to 1 if VX >= VY and 0 if not)\n",
                       reg2, reg1);
                return false;
            } else if (opcode2 == 6) {
                printf("Shifts V%x to the right by 1, then stores the least "
                       "significant bit of V%x prior to the shift into VF.\n",
                       reg1, reg1);
                return false;
            } else if (opcode2 == 7) {
                printf("Sets V%x to V%x minus V%x. VF is set to 0 when there's "
                       "an underflow, and 1 when there is not\n",
                       reg1, reg2, reg1);
                return false;
            } else if (opcode2 == 0xE) {
                printf("Shifts V%x to the left by 1, then sets VF to 1 if the "
                       "most significant bit of V%x prior to that shift was "
                       "set, or to 0 if it was unset\n",
                       reg1, reg1);
                return false;
            }
        } else if (opcode1 == 9) { // 9XY0
            const uint16_t reg1 = (instruction & 0x0F00) >> 8;
            const uint16_t reg2 = (instruction & 0x00F0) >> 4;
            printf("Skips the next instruction if V%x does not equal V%x\n",
                   reg1, reg2);
            return false;
        } else if (opcode1 == 0xA) { // ANNN
            const uint16_t addr = instruction & 0x0FFF;
            printf("sets I to 0x%x\n", addr);
            return false;
        } else if (opcode1 == 0xB) { // BNNN
            const uint16_t addr = instruction & 0x0FFF;
            printf("Jumps to the address 0X%x plus V0\n", addr);
            return false;
        } else if (opcode1 == 0xC) { // CXNN
            const uint16_t reg = (instruction & 0x0F00) >> 8;
            const uint16_t val = instruction & 0x00FF;
            printf("sets V%x to the result of a bitwise and operation on a "
                   "random number and 0X%x\n",
                   reg, val);
            return false;
        } else if (opcode1 == 0xD) { // DXYN
            const uint16_t reg1 = (instruction & 0x0F00) >> 8;
            const uint16_t reg2 = (instruction & 0x00F0) >> 4;
            const uint16_t val = instruction & 0x000F;
            printf("Draw at x=v%x y=%x with width=8 and height=0X%x\n", reg1,
                   reg2, val);
            return false;
        } else if ((instruction & 0xF0FF) == 0xE09E) {
            const uint16_t reg = (instruction & 0x0F00) >> 8;
            printf("Skips the next instruction if the key stored in V%x is "
                   "pressed\n",
                   reg);
            return false;
        } else if ((instruction & 0xF0FF) == 0xE0A1) {
            const uint16_t reg = (instruction & 0x0F00) >> 8;
            printf("Skips the next instruction if the key stored in V%x is NOT "
                   "pressed\n",
                   reg);
            return false;
        } else if ((instruction & 0xF0FF) == 0xF007) {
            const uint16_t reg = (instruction & 0x0F00) >> 8;
            printf("Sets V%x to the value of the delay timer\n", reg);
            return false;
        } else if ((instruction & 0xF0FF) == 0xF00A) {
            const uint16_t reg = (instruction & 0x0F00) >> 8;
            printf("A key press is awaited, and then stored in V%x\n", reg);
            return false;
        } else if ((instruction & 0xF0FF) == 0xF015) {
            const uint16_t reg = (instruction & 0x0F00) >> 8;
            printf("sets the delay timer to V%x\n", reg);
            return false;
        } else if ((instruction & 0xF0FF) == 0xF018) {
            const uint16_t reg = (instruction & 0x0F00) >> 8;
            printf("sets the sound timer to V%x\n", reg);
            return false;
        } else if ((instruction & 0xF0FF) == 0xF01E) {
            const uint16_t reg = (instruction & 0x0F00) >> 8;
            printf("adds V%x to I\n", reg);
            return false;
        } else if ((instruction & 0xF0FF) == 0xF029) {
            const uint16_t reg = (instruction & 0x0F00) >> 8;
            printf("Sets I to the location of the sprite for the character in "
                   "V%x\n",
                   reg);
            return false;
        } else if ((instruction & 0xF0FF) == 0xF033) {
            const uint16_t reg = (instruction & 0x0F00) >> 8;
            printf(
                "Stores the binary-coded decimal representation of V%x, with "
                "the hundreds digit in memory at location in I, the tens digit "
                "at location I+1, and the ones digit at location I+2\n",
                reg);
            return false;
        } else if ((instruction & 0xF0FF) == 0xF055) {
            const uint16_t reg = (instruction & 0x0F00) >> 8;
            printf("Stores from V0 to V%x (including V%x) in memory, starting "
                   "at address I. The offset from I is increased by 1 for each "
                   "value written, but I itself is left unmodified.\n",
                   reg, reg);
            return false;
        } else if ((instruction & 0xF0FF) == 0xF065) {
            const uint16_t reg = (instruction & 0x0F00) >> 8;
            printf(
                "Fills from V0 to V%x (including V%x) with values from memory, "
                "starting at address I. The offset from I is increased by 1 "
                "for each value read, but I itself is left unmodified\n",
                reg, reg);
            return false;
        }
    }
    printf("Unknown instruction 0X%X\n", instruction);
    return false;
}
