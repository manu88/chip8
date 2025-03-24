//
//  SDLPeripherals.cpp
//  chip8EMU
//
//  Created by Manuel Deneu on 24/03/2025.
//

#include "SDLPeripherals.hpp"
#include <string>

SDLPeripherals::SDLPeripherals() {}

void SDLPeripherals::init() {
    SDL_Init(SDL_INIT_EVERYTHING);
    SDL_CreateWindowAndRenderer(320, 640, 0, &window, &renderer);

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);
}

SDLPeripherals::~SDLPeripherals() {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void SDLPeripherals::update() {
    SDL_Event e;

    while (SDL_PollEvent(&e) != 0) {
        if (e.type == SDL_QUIT) {
            quit = true;
        }
    }
}

void SDLPeripherals::draw(uint16_t x, uint16_t y, uint16_t height, uint16_t i) {
    printf("SDL Draw at x=0X%0X, y=0X%0X height=0X%0X, i=0X%0X\n", x, y, height,
           i);
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
void SDLPeripherals::clearDisplay() {
    SDL_RenderClear(renderer);
}

bool SDLPeripherals::shouldStop(){
    return quit;
}
