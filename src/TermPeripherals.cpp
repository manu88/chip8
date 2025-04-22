//
//  TermPeripherals.cpp
//  chip8EMU
//
//  Created by Manuel Deneu on 31/03/2025.
//

#include "TermPeripherals.hpp"
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

    _ouputWin = newwin(Peripherals::SCREEN_HEIGTH, Peripherals::SCREEN_WIDTH, startY,
                  startX);
    
    startX += Peripherals::SCREEN_WIDTH + 4;
    
    _stateWin = newwin(Peripherals::SCREEN_HEIGTH, Peripherals::SCREEN_WIDTH, startY,
                  startX);
    
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
    mvwprintw(_stateWin, 1, 1, "test");
    mvwprintw(_stateWin, 2, 1, "test1");
    wrefresh(_ouputWin);
    wrefresh(_stateWin);
    refresh();
    usleep(params.timeoutMS * 1000);
}

uint8_t TermPeripherals::waitKeyPress() {
    return Peripherals::getKeyCode(wgetch(_ouputWin));
}

void TermPeripherals::clearDisplay() { _commands.clear(); }
bool TermPeripherals::shouldStop() { return false; }
