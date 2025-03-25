//
//  SDLPeripherals.cpp
//  chip8EMU
//
//  Created by Manuel Deneu on 24/03/2025.
//

#include "SDLPeripherals.hpp"
#include "Memory.hpp"
#include <string>

SDLPeripherals::SDLPeripherals() {}

void SDLPeripherals::init() {
    SDL_Init(SDL_INIT_EVERYTHING);
    SDL_CreateWindowAndRenderer(640, 320, 0, &window, &renderer);

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);
}

SDLPeripherals::~SDLPeripherals() {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

uint8_t reverse(uint8_t x) {
    x = (((x & 0xaaaaaaaa) >> 1) | ((x & 0x55555555) << 1));
    x = (((x & 0xcccccccc) >> 2) | ((x & 0x33333333) << 2));
    x = (((x & 0xf0f0f0f0) >> 4) | ((x & 0x0f0f0f0f) << 4));
    x = (((x & 0xff00ff00) >> 8) | ((x & 0x00ff00ff) << 8));
    return ((x >> 16) | (x << 16));
}

void SDLPeripherals::renderSprite(const Chip8::Memory &memory,
                                  const SDLPeripherals::DrawCommand &cmd) {
    const auto sprite = memory.getSpriteData(cmd.i);
    for (int y = 0; y < cmd.height; y++) {
        uint8_t v = sprite.data[y];
        for (int x = 0; x < 8; x++) {
            if (v & 0x0001) {

                SDL_Rect r;
                r.x = (cmd.x + 7 - x) * 10;
                r.y = (cmd.y + y) * 10;
                r.w = 10;
                r.h = 10;
                SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
                SDL_RenderFillRect(renderer, &r);
            }
            v >>= 1;
        }
    }
}

void SDLPeripherals::update(const Chip8::Memory &memory,
                            const Chip8::Peripherals::UpdateParams &params) {

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

    // Clear winow
    SDL_RenderClear(renderer);

    for (const auto &cmd : _commands) {
        renderSprite(memory, cmd);
    }

    // Render the rect to the screen
    SDL_RenderPresent(renderer);

    SDL_Event e;

    while (SDL_WaitEventTimeout(&e, params.timeoutMS) != 0) {
        if (e.type == SDL_QUIT) {
            quit = true;
        }
    }
}

void SDLPeripherals::draw(uint16_t x, uint16_t y, uint16_t height, uint16_t i) {
    printf("SDL Draw at x=0X%0X, y=0X%0X height=0X%0X, i=0X%0X\n", x, y, height,
           i);

    _commands.push_back({x, y, height, i});
}

uint8_t SDLPeripherals::waitKeyPress() {
    bool done = false;
    SDL_Event e;
    while (!done) {
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = true;
                return 0;
            }
            if (e.type == SDL_KEYUP) {
                const char key = SDL_GetKeyName(e.key.keysym.sym)[0];
                // hackish
                if (key == '0') {
                    return 0;
                }
                if (key == '1') {
                    return 1;
                }
                if (key == '2') {
                    return 2;
                }
                if (key == '3') {
                    return 3;
                }
                if (key == '4') {
                    return 4;
                }
                if (key == '5') {
                    return 5;
                }
                if (key == '6') {
                    return 6;
                }
                if (key == '7') {
                    return 7;
                }
                if (key == '8') {
                    return 8;
                }
                if (key == '9') {
                    return 9;
                }
                if (std::tolower(key) == 'a') {
                    return 10;
                }
                if (std::tolower(key) == 'b') {
                    return 11;
                }
                if (std::tolower(key) == 'c') {
                    return 12;
                }
                if (std::tolower(key) == 'd') {
                    return 13;
                }
                if (std::tolower(key) == 'e') {
                    return 14;
                }
                if (std::tolower(key) == 'f') {
                    return 15;
                }
            }
        }
    }
    return 0;
}
void SDLPeripherals::clearDisplay() { _commands.clear(); }

bool SDLPeripherals::shouldStop() { return quit; }
