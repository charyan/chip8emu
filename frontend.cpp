#include "headers/frontend.h"

#include <SDL2/SDL_ttf.h>
#include <cstring>
#include <string>

/**
 * @brief Construct a new frontend::frontend object
 * 
 * @param _winTitle Window title
 * @param _winW 
 * @param _winH 
 * @param _bufferW Width of the buffer where the pixels are drawn
 * @param _bufferH Height of the buffer where the pixels are drawn
 * @param _isDebugEnabled
 */
frontend::frontend(std::string _winTitle, uint32_t _winW, uint32_t _winH, uint32_t _bufferW, uint32_t _bufferH, bool _isDebugEnabled)
{
    isDebugEnabled = _isDebugEnabled;
    winW = _winW;
    winH = _winH;
    bufferW = _bufferW;
    bufferH = _bufferH;

    SDL_DisplayMode DM;
    SDL_GetCurrentDisplayMode(0, &DM);
    int dw = DM.w;

    window = SDL_CreateWindow(_winTitle.c_str(), (isDebugEnabled) ? 0 : SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, _winW, _winH, SDL_WINDOW_SHOWN );
    
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    tex = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_BGRA32, SDL_TEXTUREACCESS_STATIC, _bufferW, _bufferH);
    pixels = new uint32_t[_bufferW*_bufferH];

    update();

    if(isDebugEnabled){
        TTF_Init();
        debugWindow = SDL_CreateWindow("DEBUG", dw-1000, SDL_WINDOWPOS_CENTERED, 1000, 1000, SDL_WINDOW_SHOWN );
        debugRenderer = SDL_CreateRenderer(debugWindow, -1, SDL_RENDERER_ACCELERATED);
        font = TTF_OpenFont("./IBMPlexMono-Medium.ttf", 24);
    }
}

/**
 * @brief Destroy the frontend::frontend object
 * 
 */
frontend::~frontend()
{
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_DestroyTexture(tex);

    if(isDebugEnabled){
        SDL_DestroyRenderer(debugRenderer);
        SDL_DestroyWindow(debugWindow);
        TTF_CloseFont(font);
    }

    SDL_Quit();
    delete[] pixels;
}

/**
 * @brief Draw one pixel to buffer
 * 
 * @param x 
 * @param y 
 * @param color Format ARGB32 0xAARRGGBB
 */
void frontend::draw(uint32_t x, uint32_t y, uint32_t color){
    pixels[x+y*bufferW] = color;
}

/**
 * @brief Copy buffer to window
 */
void frontend::update(){
    SDL_UpdateTexture(tex, NULL, pixels, 4*bufferW);
    SDL_RenderCopy(renderer, tex, NULL, NULL);
    SDL_RenderPresent(renderer);
}

/**
 * @brief Set all pixels to color (ARGB32)
 * @param color uint32_t 0xAARRGGBB
 */
void frontend::setColor(uint32_t color){
    for(int y=0; y<(int)bufferH; ++y){
        for(int x=0; x<(int)bufferW; ++x){
            pixels[x+y*bufferW] = color;
        }
    }
    update();
}

/**
 * @brief Write text to debug window
 * 
 * @param text 
 */
void frontend::debug(std::string text){
    if(!isDebugEnabled){return;}

    SDL_RenderClear(debugRenderer);
    
    SDL_Color White = {255, 255, 255};
    // SDL_Surface* surfaceMessage =
    //     TTF_RenderText_Solid(font, text, White); 
    SDL_Surface* surfaceMessage = TTF_RenderText_Solid_Wrapped(font, text.c_str(), White, 0);
    SDL_Rect Message_rect; //create a rect
    Message_rect.x = 0;  //controls the rect's x coordinate 
    Message_rect.y = 0; // controls the rect's y coordinte
    Message_rect.w = 1000; // controls the width of the rect
    Message_rect.h = 1000; // controls the height of the rect
    SDL_Texture* Message = SDL_CreateTextureFromSurface(debugRenderer, surfaceMessage);

    SDL_FreeSurface(surfaceMessage);


    SDL_RenderCopy(debugRenderer, Message, NULL, &Message_rect);
    SDL_RenderPresent(debugRenderer);
    SDL_DestroyTexture(Message);
}
