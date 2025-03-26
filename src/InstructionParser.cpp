//
//  InstructionParser.cpp
//  chip8EMU
//
//  Created by Manuel Deneu on 26/03/2025.
//

#include "InstructionParser.hpp"

bool Chip8::InstructionParser::exec(uint16_t instruction) {
    if (instruction == 0x00E0) {
        onCLS();
        return true;
    } else if (instruction == 0x00EE) {
        onRET();
    } else {
        const uint16_t opcode1 = (instruction & 0xF000) >> 12;
        if (opcode1 == 0) { // call 0NNN
            uint16_t addr = instruction & 0x0FFF;
            onCallMachine(addr);
            return true;
        } else if (opcode1 == 1) { // 1NNN
            uint16_t addr = instruction & 0x0FFF;
            onJump(addr);
            return true;
        } else if (opcode1 == 2) { // 2NNN
            uint16_t addr = instruction & 0x0FFF;
            onCallSubroutine(addr);
            return true;
        } else if (opcode1 == 3) { // 3XNN
            uint16_t reg = (instruction & 0x0F00) >> 8;
            uint16_t val = instruction & 0x00FF;
            onSkipIfVxIsVal(reg, val);
            return true;
        } else if (opcode1 == 4) { // 4XNN
            uint16_t reg = (instruction & 0x0F00) >> 8;
            uint16_t val = instruction & 0x00FF;
            onSkipIfVxIsNotVal(reg, val);
            return true;
        } else if (opcode1 == 5) { // 5XY0
            uint16_t reg1 = (instruction & 0x0F00) >> 8;
            uint16_t reg2 = (instruction & 0x00F0) >> 4;
            onSkipIfVxIsVy(reg1, reg2);
            return true;
        } else if (opcode1 == 6) { // 6XNN
            uint16_t reg = (instruction & 0x0F00) >> 8;
            uint16_t val = instruction & 0x00FF;
            onSetVx(reg, val);
            return true;
        } else if (opcode1 == 7) { // 7XNN
            uint16_t reg = (instruction & 0x0F00) >> 8;
            uint16_t val = instruction & 0x00FF;
            onAddValToVx(reg, val);
            return true;
        } else if (opcode1 == 8) { // 8XY#
            const uint16_t opcode2 = instruction & 0x000F;
            const uint16_t reg1 = (instruction & 0x0F00) >> 8;
            const uint16_t reg2 = (instruction & 0x00F0) >> 4;

            if (opcode2 == 0) {
                onSetVxToVy(reg1, reg2);
                return true;
            } else if (opcode2 == 1) {
                onOrValToVx(reg1, reg2);
                return true;
            } else if (opcode2 == 2) {
                onAndValToVx(reg1, reg2);
                return true;
            } else if (opcode2 == 3) {
                onXOrValToVx(reg1, reg2);
                return true;
            } else if (opcode2 == 4) {
                onAddVyToVx(reg1, reg2);
                return true;
            } else if (opcode2 == 5) {
                onSubVyToVx(reg1, reg2);
                return true;
            } else if (opcode2 == 6) {
                onShiftRightVx(reg1);
                return true;
            } else if (opcode2 == 7) {
                onSubVxToVy(reg1, reg2);
                return true;
            } else if (opcode2 == 0xE) {
                onShiftLeftVx(reg1);
                return true;
            }
        } else if (opcode1 == 9) { // 9XY0
            const uint16_t reg1 = (instruction & 0x0F00) >> 8;
            const uint16_t reg2 = (instruction & 0x00F0) >> 4;
            onSkipNextIfVxIsNotVy(reg1, reg2);
            return false;
        } else if (opcode1 == 0xA) { // ANNN
            const uint16_t addr = instruction & 0x0FFF;
            onSetI(addr);
            return true;
        } else if (opcode1 == 0xB) { // BNNN
            const uint16_t addr = instruction & 0x0FFF;
            onJumpToLoc(addr);
            return true;
        } else if (opcode1 == 0xC) { // CXNN
            const uint16_t reg = (instruction & 0x0F00) >> 8;
            const uint16_t val = instruction & 0x00FF;
            onRand(reg, val);
            return true;
        } else if (opcode1 == 0xD) { // DXYN
            const uint16_t reg1 = (instruction & 0x0F00) >> 8;
            const uint16_t reg2 = (instruction & 0x00F0) >> 4;
            const uint16_t val = instruction & 0x000F;
            onDisplay(reg1, reg2, val);
            return true;
        } else if ((instruction & 0xF0FF) == 0xE09E) {
            const uint16_t reg = (instruction & 0x0F00) >> 8;
            onSkipIfKeyPressed(reg);
            return true;
        } else if ((instruction & 0xF0FF) == 0xE0A1) {
            const uint16_t reg = (instruction & 0x0F00) >> 8;
            onSkipIfKeyNotPressed(reg);
            return true;
        } else if ((instruction & 0xF0FF) == 0xF007) {
            const uint16_t reg = (instruction & 0x0F00) >> 8;
            onSetVxToDelayTimer(reg);
            return true;
        } else if ((instruction & 0xF0FF) == 0xF00A) {
            const uint16_t reg = (instruction & 0x0F00) >> 8;
            onWaitKeyPressed(reg);
            return true;
        } else if ((instruction & 0xF0FF) == 0xF015) {
            const uint16_t reg = (instruction & 0x0F00) >> 8;
            onSetDelayTimer(reg);
            return true;
        } else if ((instruction & 0xF0FF) == 0xF018) {
            const uint16_t reg = (instruction & 0x0F00) >> 8;
            onSetSoundTimer(reg);
             return true;
        } else if ((instruction & 0xF0FF) == 0xF01E) {
            const uint16_t reg = (instruction & 0x0F00) >> 8;
            onAddVxToI(reg);
            return true;
        } else if ((instruction & 0xF0FF) == 0xF029) {
            const uint16_t reg = (instruction & 0x0F00) >> 8;
            onSetIToSpriteLoc(reg);
            return true;
        } else if ((instruction & 0xF0FF) == 0xF033) {
            const uint16_t reg = (instruction & 0x0F00) >> 8;
            onStoreBCDOfVxInI(reg);
            return true;
        } else if ((instruction & 0xF0FF) == 0xF055) {
            const uint16_t reg = (instruction & 0x0F00) >> 8;
            onStoreVnInI(reg);
            return true;
        } else if ((instruction & 0xF0FF) == 0xF065) {
            const uint16_t reg = (instruction & 0x0F00) >> 8;
            onReadVnFromI(reg);
            return true;
        }
    }
    return false;
}
