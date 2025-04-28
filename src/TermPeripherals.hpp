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
    void update(const Chip8::Memory &memory, const Chip8::Registers &registers,
                const UpdateParams &params) override;
    uint8_t waitKeyPress() override;
    void clearDisplay() override;
    bool shouldStop() override;
    void signalExit() override;
    bool changeMode(bool highRes) override;
    void scroll(ScrollDirection direction, uint8_t amount) override;

  private:
    void renderSprite(const Chip8::Memory &memory, const DrawCommand &cmd);
    WINDOW *_ouputWin;
    WINDOW *_stateWin;

    bool _shouldStop = false;
    bool _highRes = false;
    int _scrollXOffset = 0;
    int _scrollYOffset = 0;
};
