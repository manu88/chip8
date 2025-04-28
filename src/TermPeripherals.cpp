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
#include <ncurses.h>
#include <string>
#include <unistd.h>

#define nscroll scroll
#undef scroll

bool TermPeripherals::init() {
    initscr();
    clear();
    noecho();
    cbreak(); /* Line buffering disabled. pass on everything */

    int startX = 4;
    int startY = 1;

    _ouputWin = newwin(Peripherals::LOW_RES_SCREEN_HEIGTH,
                       Peripherals::LOW_RES_SCREEN_WIDTH, startY, startX);

    startX += Peripherals::HIGH_RES_SCREEN_WIDTH + 4;

    _stateWin = newwin(24, 24, startY, startX);

    keypad(_ouputWin, TRUE);
    mvprintw(0, 0, "Chip8 emulator");
    refresh();
    return true;
}

TermPeripherals::~TermPeripherals() { endwin(); }

void TermPeripherals::update(const Chip8::Memory &memory,
                             const Chip8::Registers &registers,
                             const UpdateParams &params) {
    Chip8::Peripherals::update(memory, registers, params);
    box(_ouputWin, 0, 0);
    box(_stateWin, 0, 0);

    for (int x = 0; x < _currentWidth; x++) {
        for (int y = 0; y < _currentHeight; y++) {
            int xx = _scrollXOffset + x;
            int yy = _scrollYOffset + y;
            if (buffer[x][y]) {
                mvwprintw(_ouputWin, yy, xx, "#");
            }
        }
    }

    mvwprintw(_stateWin, 1, 1, "pc: %s", hex(registers.pc).c_str());
    mvwprintw(_stateWin, 2, 1, "sp: %s", hex(registers.sp).c_str());
    mvwprintw(_stateWin, 3, 1, "delay: %s", hex(registers.delayTimer).c_str());
    mvwprintw(_stateWin, 4, 1, "sound: %s", hex(registers.soundTimer).c_str());
    for (int i = 0; i < Chip8::Registers::Size; i++) {
        mvwprintw(_stateWin, 5 + i, 1, "v[%i]: %s", i,
                  hex(registers.v[i]).c_str());
    }
    mvwprintw(_stateWin, 5 + 16, 1, "mode: %s", _highRes ? "HIGH" : "LOW");
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

void TermPeripherals::changeMode(bool highRes) {
    Chip8::Peripherals::changeMode(highRes);
    wresize(_ouputWin, _currentHeight, _currentWidth);
}
