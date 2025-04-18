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

const char *choices[] = {
    "Choice 1", "Choice 2", "Choice 3", "Choice 4", "Exit",
};
int n_choices = sizeof(choices) / sizeof(char *);

void print_menu(WINDOW *menu_win, int highlight) {
    int x, y, i;

    x = 2;
    y = 2;
    box(menu_win, 0, 0);
    for (i = 0; i < n_choices; ++i) {
        if (highlight == i + 1) /* High light the present choice */
        {
            wattron(menu_win, A_REVERSE);
            mvwprintw(menu_win, y, x, "%s", choices[i]);
            wattroff(menu_win, A_REVERSE);
        } else
            mvwprintw(menu_win, y, x, "%s", choices[i]);
        ++y;
    }
    wrefresh(menu_win);
}

static void test(WINDOW *menu_win) {

    int highlight = 1;
    int choice = 0;
    int c;

    mvprintw(
        0, 0,
        "Use arrow keys to go up and down, Press enter to select a choice");
    refresh();
    print_menu(menu_win, highlight);
    while (1) {
        c = wgetch(menu_win);
        switch (c) {
        case KEY_UP:
            if (highlight == 1)
                highlight = n_choices;
            else
                --highlight;
            break;
        case KEY_DOWN:
            if (highlight == n_choices)
                highlight = 1;
            else
                ++highlight;
            break;
        case 10:
            choice = highlight;
            break;
        default:
            mvprintw(
                24, 0,
                "Charcter pressed is = %3d Hopefully it can be printed as '%c'",
                c, c);
            refresh();
            break;
        }
        print_menu(menu_win, highlight);
        if (choice != 0) /* User did a choice come out of the infinite loop */
            break;
    }
    mvprintw(23, 0, "You chose choice %d with choice string %s\n", choice,
             choices[choice - 1]);
    clrtoeol();
    refresh();
    endwin();
    exit(0);
}

bool TermPeripherals::init() {
    initscr();
    clear();
    noecho();
    cbreak(); /* Line buffering disabled. pass on everything */

    int startx = 4;
    int starty = 4;

    _win = newwin(Peripherals::SCREEN_HEIGTH, Peripherals::SCREEN_WIDTH, starty,
                  startx);
    keypad(_win, TRUE);
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
                mvwprintw(_win, yP, xP, "#");
            } else {
                mvwprintw(_win, yP, xP, " ");
            }
            v >>= 1;
        }
    }
}

void TermPeripherals::update(const Chip8::Memory &memory,
                             const Chip8::Registers &registers,
                             const UpdateParams &params) {
    refresh();
    box(_win, 0, 0);
    wrefresh(_win);
    for (const auto &cmd : _commands) {
        renderSprite(memory, cmd);
    }
    refresh();
}

void TermPeripherals::draw(uint16_t x, uint16_t y, uint16_t height,
                           uint16_t i) {

    _commands.push_back({x, y, height, i});
}

uint8_t TermPeripherals::waitKeyPress() {
    return Peripherals::getKeyCode(wgetch(_win));
}

void TermPeripherals::clearDisplay() { _commands.clear(); }
bool TermPeripherals::shouldStop() { return false; }
