#ifndef CHIP8_H
#define CHIP8_H

#include <iostream>
#include <stack>
#include <vector>

#include "frontend.h"

class chip8
{
private:
    uint8_t* ram;
    bool* display;
    uint16_t PC;
    uint16_t I;
    std::stack<uint16_t> stack;
    uint8_t delayTimer;
    uint8_t soundTimer;
    uint8_t* V;

    uint32_t displayH;
    uint32_t displayW;

    std::vector<uint16_t> lastTenInstructions;

    uint16_t fetch();
    void decode(uint16_t _instruct);
    std::string getDebugText();
    std::string hexString(uint32_t n, uint32_t width);

    frontend* F;

public:
    chip8(frontend* _F);
    ~chip8();
    void tick();
    void timerTick();
    void load(std::string filename);
};

#endif // CHIP8_H