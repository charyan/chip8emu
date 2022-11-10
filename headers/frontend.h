#ifndef FRONTEND_H
#define FRONTEND_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <string>

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

public:
    frontend(std::string _winTitle, uint32_t _winW, uint32_t _winH, uint32_t _bufferW, uint32_t _bufferH, bool isDebugEnabled);
    ~frontend();
    void draw(uint32_t x, uint32_t y, uint32_t color);
    void setColor(uint32_t color);
    void update();
    void debug(std::string text);
};

#endif // FRONTEND_H