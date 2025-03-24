//
//  SDLPeripherals.hpp
//  chip8EMU
//
//  Created by Manuel Deneu on 24/03/2025.
//
#pragma once
#include "Peripherals.hpp"
#include <SDL2/SDL.h>
#include <vector>

namespace Chip8 {
class Memory;
}

class SDLPeripherals : public Chip8::Peripherals {
  public:

    SDLPeripherals();
    ~SDLPeripherals();

    void init();

    void update(const Chip8::Memory &memory) override;
    void draw(uint16_t x, uint16_t y, uint16_t height, uint16_t i) override;
    uint8_t waitKeyPress() override;
    void clearDisplay() override;
    bool shouldStop() override;

  private:
    
    struct DrawCommand {
        uint16_t x;
        uint16_t y;
        uint16_t height;
        uint16_t i;
    };
    std::vector<DrawCommand> _commands;
    
    void renderSprite(const Chip8::Memory &memory, const DrawCommand& cmd);
    SDL_Window *window = NULL;
    SDL_Renderer *renderer = NULL;

    bool quit = false;
};
