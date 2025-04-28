//
//  SDLPeripherals.cpp
//  chip8EMU
//
//  Created by Manuel Deneu on 24/03/2025.
//

#include "SDLPeripherals.hpp"
#include "Emulator.hpp"
#include "HexHelpers.hpp"
#include "Memory.hpp"
#include <SDL2/SDL_ttf.h>
#include <string>

// we draw the 'screen' with a 10,10 offset from top-right side of screen
#define OFFSET (int)10
#define FONT_SIZE 16

SDLPeripherals::SDLPeripherals() {
    TTF_Init();
    _font = TTF_OpenFont("/opt/Arial.ttf", FONT_SIZE);
}

bool SDLPeripherals::init() {
    SDL_Init(SDL_INIT_EVERYTHING);

    int width = (int)Peripherals::LOW_RES_SCREEN_WIDTH * LOW_RES_SCALE_FACTOR;

    width += 300; // more space for debug infos
    int height = (int)Peripherals::LOW_RES_SCREEN_HEIGTH * LOW_RES_SCALE_FACTOR;
    height += 20;
    SDL_CreateWindowAndRenderer(width, height, 0, &window, &renderer);

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);
    return Peripherals::init();
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

void SDLPeripherals::update(const Chip8::Memory &memory,
                            const Chip8::Registers &registers,
                            const Chip8::Peripherals::UpdateParams &params) {
    Chip8::Peripherals::update(memory, registers, params);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

    SDL_RenderClear(renderer);

    // frame around chip8 'screen'
    SDL_Rect r;
    r.x = OFFSET;
    r.y = OFFSET;
    r.w = (int)Peripherals::LOW_RES_SCREEN_WIDTH * LOW_RES_SCALE_FACTOR;
    r.h = (int)Peripherals::LOW_RES_SCREEN_HEIGTH * LOW_RES_SCALE_FACTOR;
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderDrawRect(renderer, &r);

    int factor =
        _highRes ? (int)HIGH_RES_SCALE_FACTOR : (int)LOW_RES_SCALE_FACTOR;

    for (int x = 0; x < _currentWidth; x++) {
        for (int y = 0; y < _currentHeight; y++) {
            int xx = _scrollXOffset + OFFSET;
            int yy = _scrollYOffset + OFFSET;
            if (buffer[x][y]) {
                SDL_Rect r;
                r.x = xx + (x * factor);
                r.y = yy + (y * factor);
                r.w = factor;
                r.h = factor;
                SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
                SDL_RenderFillRect(renderer, &r);
            }
        }
    }

    // render machine stats
    const int startX =
        (int)Peripherals::LOW_RES_SCREEN_WIDTH * LOW_RES_SCALE_FACTOR + 20;
    const int startY = 10;
    renderText(renderer, startX, startY, "pc: " + hex(registers.pc), _font);
    int inc = 1;
    renderText(renderer, startX, startY + (FONT_SIZE * inc),
               "sp: " + hex(registers.sp), _font);
    inc += 1;
    renderText(renderer, startX, startY + (FONT_SIZE * inc),
               "delay: " + hex(registers.delayTimer), _font);
    inc += 1;
    renderText(renderer, startX, startY + (FONT_SIZE * inc),
               "sound: " + hex(registers.soundTimer), _font);
    inc += 1;
    for (int i = 0; i < 16; i += 2) {
        std::string s = "v" + std::to_string(i) + ": " + hex(registers.v[i]);
        s += " v" + std::to_string(i + 1) + ": " + hex(registers.v[i + 1]);
        renderText(renderer, startX,
                   startY + (FONT_SIZE * 4) + (FONT_SIZE / 2 * i), s, _font);
    }
    inc += 8;
    renderText(renderer, startX, startY + (FONT_SIZE * inc),
               std::string("mode: ") + (_highRes ? "HIGH" : "LOW"), _font);
    inc += 1;
    SDL_RenderPresent(renderer);

    SDL_Event e;

    while (SDL_WaitEventTimeout(&e, params.timeoutMS) != 0) {
        if (e.type == SDL_QUIT) {
            _shouldStop = true;
        }
    }
}

uint8_t SDLPeripherals::waitKeyPress() {
    bool done = false;
    SDL_Event e;
    while (!done) {
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                _shouldStop = true;
                return 0;
            }
            if (e.type == SDL_KEYUP) {
                const char key = SDL_GetKeyName(e.key.keysym.sym)[0];
                return Peripherals::getKeyCode(key);
            }
        }
    }
    return 0;
}

void SDLPeripherals::clearDisplay() { _commands.clear(); }

bool SDLPeripherals::shouldStop() { return _shouldStop; }

void SDLPeripherals::signalExit() { _shouldStop = true; }

void SDLPeripherals::changeMode(bool highRes) {
    printf("SDL: switch mode from %i to %i\n", _highRes, highRes);
    _highRes = highRes;
}
