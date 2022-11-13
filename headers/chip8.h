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

    bool keyPressed_0;
    bool keyPressed_1;
    bool keyPressed_2;
    bool keyPressed_3;
    bool keyPressed_4;
    bool keyPressed_5;
    bool keyPressed_6;
    bool keyPressed_7;
    bool keyPressed_8;
    bool keyPressed_9;
    bool keyPressed_a;
    bool keyPressed_b;
    bool keyPressed_c;
    bool keyPressed_d;
    bool keyPressed_e;
    bool keyPressed_f;
};

#endif // CHIP8_H