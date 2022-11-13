#ifndef FRONTEND_H
#define FRONTEND_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <string>
#include <vector>

#include "../imgui/imgui.h"
#include "../imgui/backends/imgui_impl_sdl.h"
#include "../imgui/backends/imgui_impl_sdlrenderer.h"

#include "Application.h"

#include <stdio.h>
#include <filesystem>

#if !SDL_VERSION_ATLEAST(2,0,17)
#error This backend requires SDL 2.0.17+ because of SDL_RenderGeometry() function
#endif

class Application;

class frontend
{
private:
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Texture* tex;
    uint32_t* pixels;
    uint32_t winW;
    uint32_t winH;
    uint32_t bufferW;
    uint32_t bufferH;

    SDL_Window* debugWindow;
    SDL_Renderer* debugRenderer;
    TTF_Font* font;

    bool isDebugEnabled;

    std::vector<std::string> roms;
    void getRoms();



public:
    Application* app;
    frontend(std::string _winTitle, uint32_t _winW, uint32_t _winH, uint32_t _bufferW, uint32_t _bufferH, bool isDebugEnabled);
    ~frontend();
    void draw(uint32_t x, uint32_t y, uint32_t color);
    void setColor(uint32_t color);
    void update();

    void newImGuiFrame();
    
    void drawChip8Window();
    void drawMainMenuBar();
    void drawSystemWindow();
    void drawMemoryEditor();
    void clear();

    // State
    int clockFreq;

    bool chip8Window_isOpen;
    bool chip8Window_isPaused;

    bool systemWindow_isOpen;

    bool memoryWindow_isOpen;
};

#endif // FRONTEND_H