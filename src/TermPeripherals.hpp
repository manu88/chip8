//
//  TermPeripherals.hpp
//  chip8EMU
//
//  Created by Manuel Deneu on 31/03/2025.
//

#pragma once

#include "Peripherals.hpp"
#include <stdint.h>

typedef struct _win_st WINDOW;

class TermPeripherals : public Chip8::Peripherals {
  public:
    enum { LOW_RES_SCALE_FACTOR = 10 };
    enum { HIGH_RES_SCALE_FACTOR = 5 };

    bool init() override;
    ~TermPeripherals();
    bool update(const Chip8::Memory &memory, const Chip8::Registers &registers,
                const UpdateParams &params) override;
    uint8_t waitKeyPress() override;
    void clearDisplay() override;
    void changeMode(bool highRes) override;

  private:
    WINDOW *_ouputWin;
    WINDOW *_stateWin;
};
