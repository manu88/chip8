//
//  TermPeripherals.hpp
//  chip8EMU
//
//  Created by Manuel Deneu on 31/03/2025.
//

#pragma once

#include "Peripherals.hpp"
#include <stdint.h>

typedef struct _win_st WINDOW; // forward def

class TermPeripherals : public Chip8::Peripherals {
  public:
    enum { LOW_RES_SCALE_FACTOR = 10 };
    enum { HIGH_RES_SCALE_FACTOR = 5 };

    bool init() override;
    void reset() override;
    TermPeripherals(const Chip8::Config &conf) : Chip8::Peripherals(conf) {}
    ~TermPeripherals();

    bool update(Chip8::CPU &cpu, const UpdateParams &params) override;
    uint8_t waitKeyPress() override;
    void changeMode(bool highRes) override;
    void clearDisplay() override;

  private:
    WINDOW *_outputWin;
    WINDOW *_stateWin;
    WINDOW *_debugWin;
};
