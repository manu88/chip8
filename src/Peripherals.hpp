//
//  Peripherals.hpp
//  chip8EMU
//
//  Created by Manuel Deneu on 24/03/2025.
//

#pragma once
#include <random>
#include <stdint.h>
#include <vector>

namespace Chip8 {
class Memory;
class Registers;

class Peripherals {
  public:
    enum { LOW_RES_SCREEN_WIDTH = 64 };
    enum { LOW_RES_SCREEN_HEIGTH = 32 };

    enum { HIGH_RES_SCREEN_WIDTH = 128 };
    enum { HIGH_RES_SCREEN_HEIGTH = 64 };

    Peripherals() : _rng(_randomDevice()) {}
    virtual ~Peripherals() {}

    virtual bool init();

    struct UpdateParams {
        int timeoutMS;
        long frameId;
    };
    virtual void signalExit() {}
    virtual void changeMode(bool highRes);
    virtual void update(const Memory &memory, const Registers &registers,
                        const UpdateParams &params);
    void draw(uint16_t x, uint16_t y, uint16_t height, uint16_t i);
    virtual uint8_t waitKeyPress();
    virtual std::vector<uint8_t> getKeysPressed();
    virtual void clearDisplay();
    virtual bool shouldStop() { return false; }
    virtual uint16_t getRand();

    enum ScrollDirection {
        Left,
        Right,
        Down,
    };
    virtual void scroll(ScrollDirection direction, uint8_t amount);

    static uint8_t getKeyCode(char key);

  protected:
    
    uint8_t buffer[HIGH_RES_SCREEN_WIDTH][HIGH_RES_SCREEN_HEIGTH];
    
    struct DrawCommand {
        uint16_t x;
        uint16_t y;
        uint16_t height;
        uint16_t i;
    };
    std::vector<DrawCommand> _commands;

    bool _shouldStop = false;
    bool _highRes = false;
    int _scrollXOffset = 0;
    int _scrollYOffset = 0;
    int _currentWidth = LOW_RES_SCREEN_WIDTH;
    int _currentHeight = LOW_RES_SCREEN_HEIGTH;
    
  private:
    void renderSprite(const Chip8::Memory &memory, const DrawCommand &cmd);
    
    std::random_device _randomDevice;
    std::mt19937 _rng;
    std::uniform_int_distribution<uint8_t> _uint8Distrib;
};
} // namespace Chip8
