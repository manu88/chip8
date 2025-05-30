//
//  InstructionParser.cpp
//  chip8EMU
//
//  Created by Manuel Deneu on 26/03/2025.
//

#include "InstructionParser.hpp"
#include <assert.h>

bool Chip8::InstructionParser::exec(uint16_t instruction) {
    currentInstruction = instruction;
    if (instruction == 0) {
        return onNOP();
    }
    if (instruction == 0x00E0) {
        return onCLS();
    } else if (instruction == 0x00EE) {
        return onRET();
    } else {
        const uint16_t opcode1 = (instruction & 0xF000) >> 12;
        if (opcode1 == 0) {
            if (_conf.superInstructions && instruction == 0X00FD) {
                return onExit();
            } else if (instruction == 0X00FB) {
                // 00FB - SCR : Scroll display 4 pixels right
                return onSCR();
            } else if (instruction == 0X00FC) {
                // 00FC - SCL : Scroll display 4 pixels left
                return onSCL();
            } else if ((instruction & 0XFFF0) == 0X00C0) {
                const uint16_t n = instruction & 0x000F;
                return onScrollDown(n);
            } else if (instruction == 0X00FE) {
                // 00FE - LOW : Disable extended screen mode
                return onLowRes();
            } else if (instruction == 0X00FF) {
                /* 00FF - HIGH : Enable extended screen mode for full-screen
                 graphics Enable 128x64 high resolution graphics mode.. A sprite
                 of 16x16 size is available.
                 */
                return onHighRes();
            }
            // call 0NNN
            uint16_t addr = instruction & 0x0FFF;
            return onCallMachine(addr);

        } else if (opcode1 == 1) { // 1NNN
            uint16_t addr = instruction & 0x0FFF;
            return onJump(addr);

        } else if (opcode1 == 2) { // 2NNN
            uint16_t addr = instruction & 0x0FFF;
            return onCallSubroutine(addr);

        } else if (opcode1 == 3) { // 3XNN
            uint16_t reg = (instruction & 0x0F00) >> 8;
            uint16_t val = instruction & 0x00FF;
            return onSkipIfVxIsVal(reg, val);

        } else if (opcode1 == 4) { // 4XNN
            uint16_t reg = (instruction & 0x0F00) >> 8;
            uint16_t val = instruction & 0x00FF;
            return onSkipIfVxIsNotVal(reg, val);

        } else if (opcode1 == 5) { // 5XY0
            uint16_t reg1 = (instruction & 0x0F00) >> 8;
            uint16_t reg2 = (instruction & 0x00F0) >> 4;
            return onSkipIfVxIsVy(reg1, reg2);

        } else if (opcode1 == 6) { // 6XNN
            uint16_t reg = (instruction & 0x0F00) >> 8;
            uint16_t val = instruction & 0x00FF;
            return onSetVx(reg, val);

        } else if (opcode1 == 7) { // 7XNN
            uint16_t reg = (instruction & 0x0F00) >> 8;
            uint16_t val = instruction & 0x00FF;
            return onAddValToVx(reg, val);

        } else if (opcode1 == 8) { // 8XY#
            const uint16_t opcode2 = instruction & 0x000F;
            const uint16_t reg1 = (instruction & 0x0F00) >> 8;
            const uint16_t reg2 = (instruction & 0x00F0) >> 4;

            if (opcode2 == 0) {
                return onSetVxToVy(reg1, reg2);

            } else if (opcode2 == 1) {
                return onOrValToVx(reg1, reg2);

            } else if (opcode2 == 2) {
                return onAndValToVx(reg1, reg2);

            } else if (opcode2 == 3) {
                return onXOrValToVx(reg1, reg2);

            } else if (opcode2 == 4) {
                return onAddVyToVx(reg1, reg2);

            } else if (opcode2 == 5) {
                return onSubVyToVx(reg1, reg2);

            } else if (opcode2 == 6) {
                return onShiftRightVx(reg1);

            } else if (opcode2 == 7) {
                return onSubVxToVy(reg1, reg2);

            } else if (opcode2 == 0xE) {
                return onShiftLeftVx(reg1);
            }
        } else if (opcode1 == 9) { // 9XY0
            const uint16_t reg1 = (instruction & 0x0F00) >> 8;
            const uint16_t reg2 = (instruction & 0x00F0) >> 4;
            return onSkipNextIfVxIsNotVy(reg1, reg2);
        } else if (opcode1 == 0xA) { // ANNN
            const uint16_t addr = instruction & 0x0FFF;
            return onSetI(addr);

        } else if (opcode1 == 0xB) { // BNNN
            const uint16_t addr = instruction & 0x0FFF;
            return onJumpToLoc(addr);

        } else if (opcode1 == 0xC) { // CXNN
            const uint16_t reg = (instruction & 0x0F00) >> 8;
            const uint16_t val = instruction & 0x00FF;
            return onRand(reg, val);

        } else if (opcode1 == 0xD) { // DXYN
            const uint16_t reg1 = (instruction & 0x0F00) >> 8;
            const uint16_t reg2 = (instruction & 0x00F0) >> 4;
            const uint16_t val = instruction & 0x000F;
            if (_conf.superInstructions && val == 0) {
                return onSuperChipDisplay(reg1, reg2);
            }
            return onDisplay(reg1, reg2, val);

        } else if ((instruction & 0xF0FF) == 0xE09E) {
            const uint16_t reg = (instruction & 0x0F00) >> 8;
            return onSkipIfKeyPressed(reg);

        } else if ((instruction & 0xF0FF) == 0xE0A1) {
            const uint16_t reg = (instruction & 0x0F00) >> 8;
            return onSkipIfKeyNotPressed(reg);

        } else if ((instruction & 0xF0FF) == 0xF007) {
            const uint16_t reg = (instruction & 0x0F00) >> 8;
            return onSetVxToDelayTimer(reg);

        } else if ((instruction & 0xF0FF) == 0xF00A) {
            const uint16_t reg = (instruction & 0x0F00) >> 8;
            return onWaitKeyPressed(reg);

        } else if ((instruction & 0xF0FF) == 0xF015) {
            const uint16_t reg = (instruction & 0x0F00) >> 8;
            return onSetDelayTimer(reg);

        } else if ((instruction & 0xF0FF) == 0xF018) {
            const uint16_t reg = (instruction & 0x0F00) >> 8;
            return onSetSoundTimer(reg);

        } else if ((instruction & 0xF0FF) == 0xF01E) {
            const uint16_t reg = (instruction & 0x0F00) >> 8;
            return onAddVxToI(reg);

        } else if ((instruction & 0xF0FF) == 0xF029) {
            const uint16_t reg = (instruction & 0x0F00) >> 8;
            return onSetIToSpriteLoc(reg);
        } else if (_conf.superInstructions &&
                   (instruction & 0XF0FF) == 0XF030) {
            const uint16_t reg = (instruction & 0x0F00) >> 8;
            return onSetIToBigSpriteLoc(reg);
        } else if ((instruction & 0xF0FF) == 0xF033) {
            const uint16_t reg = (instruction & 0x0F00) >> 8;
            return onStoreBCDOfVxInI(reg);

        } else if ((instruction & 0xF0FF) == 0xF055) {
            const uint16_t reg = (instruction & 0x0F00) >> 8;
            return onStoreVnInI(reg);

        } else if ((instruction & 0xF0FF) == 0xF065) {
            const uint16_t reg = (instruction & 0x0F00) >> 8;
            return onReadVnFromI(reg);
        } else if (_conf.superInstructions &&
                   (instruction & 0XF0FF) == 0XF075) {
            const uint8_t reg = (instruction & 0x0F00) >> 8;
            if (reg > 7) {
                return false;
            }
            return onSaveFlagRegister(reg);
        } else if (_conf.superInstructions &&
                   (instruction & 0XF0FF) == 0XF085) {
            const uint8_t reg = (instruction & 0x0F00) >> 8;
            if (reg > 7) {
                return false;
            }
            return onLoadFlagRegister(reg);
        }
    }
    return false;
}
