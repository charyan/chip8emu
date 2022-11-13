#if !defined(APPLICATION_H)
#define APPLICATION_H

#include <SDL2/SDL.h>
#include <iostream>

#include "frontend.h"
#include "chip8.h"

#include "../imgui/imgui.h"
#include "../imgui/backends/imgui_impl_sdl.h"
#include "../imgui/backends/imgui_impl_sdlrenderer.h"

#if !SDL_VERSION_ATLEAST(2,0,17)
#error This backend requires SDL 2.0.17+ because of SDL_RenderGeometry() function
#endif

class frontend;
class chip8;

class Application
{
private:
    
public:
    frontend* fr;
    chip8* ch;
    Application();
    ~Application();
};


#endif // APPLICATION_H
