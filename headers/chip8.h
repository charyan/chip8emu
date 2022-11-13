#ifndef CHIP8_H
#define CHIP8_H

#include <iostream>
#include <stack>
#include <vector>

#include "Application.h"

class Application;

class chip8
{
private:
    bool* display;

    uint32_t displayH;
    uint32_t displayW;

    uint16_t fetch();
    void decode(uint16_t _instruct);
    std::string hexString(uint32_t n, uint32_t width);


public:
    std::stack<uint16_t> stack;
    uint8_t delayTimer;
    uint8_t soundTimer;
    uint8_t* V;
    uint16_t PC;
    uint16_t I;
    uint8_t* ram;
    Application* app;
    chip8();
    ~chip8();
    void tick();
    void timerTick();
    void load(std::string filename);
    void reset();
};

#endif // CHIP8_H