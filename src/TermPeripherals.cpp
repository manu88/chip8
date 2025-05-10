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
#include "Rom.hpp"
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

    _outputWin = newwin(Peripherals::LOW_RES_SCREEN_HEIGTH,
                        Peripherals::LOW_RES_SCREEN_WIDTH, startY, startX);

    startX += Peripherals::LOW_RES_SCREEN_WIDTH + 4;

    _stateWin = newwin(24, 24, startY, startX);
    if (_conf.debugInstructions) {
        startY += 24;
        _debugWin = newwin(4, 30, startY, startX);
    }

    keypad(_outputWin, TRUE);
    nodelay(_outputWin, TRUE);
    mvprintw(0, 0, "Chip8 emulator");
    refresh();
    return Peripherals::init();
}

void TermPeripherals::reset() {
    Peripherals::reset();
    clearDisplay();
}

TermPeripherals::~TermPeripherals() { endwin(); }

bool TermPeripherals::update(Chip8::CPU &cpu, const UpdateParams &params) {
    Chip8::Peripherals::update(cpu, params);
    box(_outputWin, 0, 0);
    box(_stateWin, 0, 0);
    if (_conf.debugInstructions) {
        box(_debugWin, 0, 0);
    }

    for (int x = 0; x < _currentWidth; x++) {
        for (int y = 0; y < _currentHeight; y++) {
            int xx = _scrollXOffset + x;
            int yy = _scrollYOffset + y;
            if (buffer[x][y]) {
                mvwprintw(_outputWin, yy, xx, "#");
            }
        }
    }

    mvwprintw(_stateWin, 1, 1, "pc: %s", hex(cpu.getRegisters().pc).c_str());
    mvwprintw(_stateWin, 2, 1, "sp: %s", hex(cpu.getRegisters().sp).c_str());
    mvwprintw(_stateWin, 3, 1, "delay: %s",
              hex(cpu.getRegisters().delayTimer).c_str());
    mvwprintw(_stateWin, 4, 1, "sound: %s",
              hex(cpu.getRegisters().soundTimer).c_str());
    for (int i = 0; i < Chip8::Registers::Size; i++) {
        mvwprintw(_stateWin, 5 + i, 1, "v[%i]: %s", i,
                  hex(cpu.getRegisters().v[i]).c_str());
    }
    mvwprintw(_stateWin, 5 + 16, 1, "mode: %s", _highRes ? "HIGH" : "LOW");
    wrefresh(_outputWin);
    wrefresh(_stateWin);
    if (_conf.debugInstructions) {
        mvwprintw(_debugWin, 1, 1,
                  cpu.debuggerIsPaused() ? "PAUSED  " : "RUNNING");
        const std::string code = cpu.getMemory().getRom()->getDebugSymbols().at(
            cpu.getRegisters().pc);
        mvwprintw(_debugWin, 2, 1, code.c_str());
        wrefresh(_debugWin);
    }

    if (_conf.debugInstructions) {
        int key = wgetch(_outputWin);
        if (key == 'n') {
            cpu.debuggerStepNext();
        } else if (key == 'r') {
            cpu.reset();
        } else if (key == ' ') {
            if (cpu.debuggerIsPaused()) {
                cpu.resumeDebugger();
            } else {
                cpu.pauseDebugger();
            }
        }
    }
    refresh();
    usleep(params.timeoutMS * 1000);
    return true;
}

uint8_t TermPeripherals::waitKeyPress() {
    mvprintw(1, 0, "waiting input");
    refresh();
    int key = wgetch(_outputWin);
    mvprintw(1, 0, "             ");
    refresh();
    return Peripherals::getKeyCode(key);
}

void TermPeripherals::clearDisplay() {
    Peripherals::clearDisplay();
    werase(_outputWin);
    werase(_stateWin);
    if (_conf.debugInstructions) {
        werase(_debugWin);
    }
}

void TermPeripherals::changeMode(bool highRes) {
    Chip8::Peripherals::changeMode(highRes);
    wresize(_outputWin, _currentHeight, _currentWidth);
}
