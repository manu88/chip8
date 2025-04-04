//
//  SDLPeripherals.cpp
//  chip8EMU
//
//  Created by Manuel Deneu on 24/03/2025.
//

#include "SDLPeripherals.hpp"
#include "Emulator.h"
#include "Memory.hpp"
#include <string>

#include <SDL2/SDL_ttf.h>

// we draw the 'screen' with a 10,10 offset from top-right side of screen
#define OFFSET (int)10
#define FONT_SIZE 16

SDLPeripherals::SDLPeripherals() {
    TTF_Init();
    _font = TTF_OpenFont("/opt/Arial.ttf", FONT_SIZE);
}

void SDLPeripherals::init() {
    SDL_Init(SDL_INIT_EVERYTHING);

    int width = (int)Peripherals::SCREEN_WIDTH * SCALE_FACTOR;

    width += 300; // more space for debug infos
    int height = (int)Peripherals::SCREEN_HEIGTH * SCALE_FACTOR;
    height += 20;
    SDL_CreateWindowAndRenderer(width, height, 0, &window, &renderer);

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);
}

SDLPeripherals::~SDLPeripherals() {
    TTF_CloseFont(_font);
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
                r.x = OFFSET + (cmd.x + 7 - x) * SCALE_FACTOR;
                r.y = OFFSET + (cmd.y + y) * SCALE_FACTOR;
                r.w = SCALE_FACTOR;
                r.h = SCALE_FACTOR;
                SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
                SDL_RenderFillRect(renderer, &r);
            }
            v >>= 1;
        }
    }
}

static void renderText(SDL_Renderer *renderer, int x, int y,
                       const std::string &text, TTF_Font *font) {
    int text_width;
    int text_height;

    SDL_Color textColor = {255, 255, 255, 0};

    SDL_Surface *surface = TTF_RenderText_Solid(font, text.c_str(), textColor);
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    text_width = surface->w;
    text_height = surface->h;
    SDL_FreeSurface(surface);
    SDL_Rect rect;
    rect.x = x;
    rect.y = y;
    rect.w = text_width;
    rect.h = text_height;

    SDL_RenderCopy(renderer, texture, NULL, &rect);
}

static std::string hex(uint16_t value) {
    char str[16];
    char *p = &str[16];
    do {
        p--;
        uint32_t digit = value % 16;
        value /= 16;
        *p = digit >= 10 ? 'a' + (digit - 10) : '0' + digit;
    } while (value > 0);
    p--;
    *p = 'x';
    p--;
    *p = '0';
    return std::string(p, &str[16] - p);
}

void SDLPeripherals::update(const Chip8::Memory &memory,
                            const Chip8::Registers &registers,
                            const Chip8::Peripherals::UpdateParams &params) {

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

    SDL_RenderClear(renderer);

    // frame around chip8 'screen'
    SDL_Rect r;
    r.x = OFFSET;
    r.y = OFFSET;
    r.w = (int)Peripherals::SCREEN_WIDTH * SCALE_FACTOR;
    r.h = (int)Peripherals::SCREEN_HEIGTH * SCALE_FACTOR;
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderDrawRect(renderer, &r);

    for (const auto &cmd : _commands) {
        renderSprite(memory, cmd);
    }

    // render machine stats
    const int startX = (int)Peripherals::SCREEN_WIDTH * SCALE_FACTOR + 20;
    const int startY = 10;
    renderText(renderer, startX, startY, "pc: " + hex(registers.pc), _font);
    renderText(renderer, startX, startY + FONT_SIZE, "sp: " + hex(registers.sp),
               _font);
    for (int i = 0; i < 16; i += 2) {
        std::string s = "v" + std::to_string(i) + ": " + hex(registers.v[i]);
        s += " v" + std::to_string(i + 1) + ": " + hex(registers.v[i + 1]);

        renderText(renderer, startX,
                   startY + (FONT_SIZE * 2) + (FONT_SIZE / 2 * i), s, _font);
    }

    SDL_RenderPresent(renderer);

    SDL_Event e;

    while (SDL_WaitEventTimeout(&e, params.timeoutMS) != 0) {
        if (e.type == SDL_QUIT) {
            quit = true;
        }
    }
}

void SDLPeripherals::draw(uint16_t x, uint16_t y, uint16_t height, uint16_t i) {
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
