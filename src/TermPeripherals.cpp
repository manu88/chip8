//
//  TermPeripherals.cpp
//  chip8EMU
//
//  Created by Manuel Deneu on 31/03/2025.
//

#include "TermPeripherals.hpp"
#include "Emulator.hpp"
#include "HexHelpers.hpp"
#include "Memory.hpp"
#include <locale.h>
#include <string>
#include <unistd.h>

bool TermPeripherals::init() {
    initscr();
    clear();
    noecho();
    cbreak(); /* Line buffering disabled. pass on everything */

    int startX = 4;
    int startY = 4;

    _ouputWin = newwin(Peripherals::LOW_RES_SCREEN_HEIGTH,
                       Peripherals::LOW_RES_SCREEN_WIDTH, startY, startX);

    startX += Peripherals::LOW_RES_SCREEN_WIDTH + 4;

    _stateWin = newwin(24, 24, startY, startX);

    keypad(_ouputWin, TRUE);
    mvprintw(0, 0, "Chip8 emulator");
    refresh();
    return true;
}

TermPeripherals::~TermPeripherals() { endwin(); }

void TermPeripherals::renderSprite(const Chip8::Memory &memory,
                                   const DrawCommand &cmd) {
    const auto sprite = memory.getSpriteData(cmd.i);
    for (int y = 0; y < cmd.height; y++) {
        uint8_t v = sprite.data[y];
        for (int x = 0; x < 8; x++) {
            int xP = (cmd.x + 7 - x);
            int yP = (cmd.y + y);
            if (v & 0x0001) {
                mvwprintw(_ouputWin, yP, xP, "#");
            } else {
                mvwprintw(_ouputWin, yP, xP, " ");
            }
            v >>= 1;
        }
    }
}

void TermPeripherals::update(const Chip8::Memory &memory,
                             const Chip8::Registers &registers,
                             const UpdateParams &params) {
    box(_ouputWin, 0, 0);
    box(_stateWin, 0, 0);

    for (const auto &cmd : _commands) {
        renderSprite(memory, cmd);
    }
    mvwprintw(_stateWin, 1, 1, "pc: %s", hex(registers.pc).c_str());
    mvwprintw(_stateWin, 2, 1, "sp: %s", hex(registers.sp).c_str());
    mvwprintw(_stateWin, 3, 1, "delay: %s", hex(registers.delayTimer).c_str());
    mvwprintw(_stateWin, 4, 1, "sound: %s", hex(registers.soundTimer).c_str());
    for (int i = 0; i < Chip8::Registers::Size; i++) {
        mvwprintw(_stateWin, 5 + i, 1, "v[%i]: %s", i,
                  hex(registers.v[i]).c_str());
    }
    wrefresh(_ouputWin);
    wrefresh(_stateWin);
    refresh();
    usleep(params.timeoutMS * 1000);
}

uint8_t TermPeripherals::waitKeyPress() {
    mvprintw(1, 0, "waiting input");
    refresh();
    int key = wgetch(_ouputWin);
    mvprintw(1, 0, "             ");
    refresh();
    return Peripherals::getKeyCode(key);
}

void TermPeripherals::clearDisplay() { _commands.clear(); }

bool TermPeripherals::shouldStop() { return _shouldStop; }

void TermPeripherals::signalExit() { _shouldStop = true; }
