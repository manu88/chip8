//
//  TermPeripherals.hpp
//  chip8EMU
//
//  Created by Manuel Deneu on 31/03/2025.
//

#pragma once

#include "Peripherals.hpp"
#include <ncurses.h>

typedef struct _win_st WINDOW;

class TermPeripherals : public Chip8::Peripherals {
  public:
    bool init() override;
    ~TermPeripherals();
    void update(const Chip8::Memory &memory, const Chip8::Registers &registers,
                const UpdateParams &params) override;
    uint8_t waitKeyPress() override;
    void clearDisplay() override;
    bool shouldStop() override;

  private:
    void renderSprite(const Chip8::Memory &memory, const DrawCommand &cmd);
    WINDOW *_ouputWin;
    WINDOW *_stateWin;
};
