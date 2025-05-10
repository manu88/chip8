//
//  SDLPeripherals.hpp
//  chip8EMU
//
//  Created by Manuel Deneu on 24/03/2025.
//
#pragma once
#include "Peripherals.hpp"
#include <SDL2/SDL.h>

typedef struct TTF_Font TTF_Font;

namespace Chip8 {
class Memory;
}

class SDLPeripherals : public Chip8::Peripherals {
  public:
    // Make sure that LOW_RES_SCREEN_WIDTH * LOW_RES_SCALE_FACTOR EQUALS
    // HIGH_RES_SCREEN_WIDTH * HIGH_RES_SCALE_FACTOR and LOW_RES_SCREEN_HEIGHT *
    // LOW_RES_SCALE_FACTOR EQUALS HIGH_RES_SCREEN_HEIGHT *
    // HIGH_RES_SCALE_FACTOR so that the SDL window's size remains the same when
    // switching mode.
    enum { LOW_RES_SCALE_FACTOR = 10 };
    enum { HIGH_RES_SCALE_FACTOR = 5 };

    SDLPeripherals(const Chip8::Config &conf);

    ~SDLPeripherals();

    bool init() override;

    bool update(Chip8::CPU &cpu,
                const Chip8::Peripherals::UpdateParams &params) override;

    uint8_t waitKeyPress() override;

  private:
    int drawStats(const Chip8::Registers &registers);
    void drawDebugger(Chip8::CPU &cpu, int startY);

    SDL_Window *window = NULL;
    SDL_Renderer *renderer = NULL;

    TTF_Font *_font;
};
