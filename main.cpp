
#define IS_DEBUG_MODE_ON true

#include <iostream>

#include "headers/chip8.h"
#include "headers/frontend.h"

#include <SDL2/SDL.h>
#include <cstdio>

const int C8_WIDTH = 64;
const int C8_HEIGHT = 32;

const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;

int main(int argc, char* args[])
{
    srand(time(0)); // Initialize random once for whole program execution

    frontend fr = frontend("chip8emu", WINDOW_WIDTH, WINDOW_HEIGHT, C8_WIDTH, C8_HEIGHT, IS_DEBUG_MODE_ON);
	chip8 c = chip8(&fr);
    
    fr.setColor(0);

    fr.update();

    if(argc==2){
        c.load(args[1]);
    } else {
        if(IS_DEBUG_MODE_ON){
            c.load("danm8ku.ch8");
        } else {
            std::cerr << "Usage: " << args[0] << " ROM" << std::endl;
        }
    }


    SDL_Event e;

    uint64_t tickTimer = 0;
    
    uint64_t lastTime = 1000;

    // Used for delayTimer and soundTimer
    uint64_t timerTimer = 0;

    bool pause = false;

    while(true){
        while(SDL_PollEvent(&e) == 1){
            if(e.type==SDL_WINDOWEVENT && e.window.event==SDL_WINDOWEVENT_CLOSE){
                goto quit;
            }

            if(e.type==SDL_KEYDOWN && e.key.keysym.sym == SDLK_SPACE){
                pause = !pause;
            }
        }
        
        if(!pause){
            if(lastTime-timerTimer >= (1000/60)){
                c.timerTick();
                timerTimer = lastTime;
            }

            if(lastTime-tickTimer >= 0){
                c.tick();
                tickTimer = lastTime;
            }
            
            lastTime = SDL_GetTicks64();
        }
    }

    quit:

    // SDL_Event e; bool quit = false; while (quit == false){while(SDL_PollEvent(&e)){if(e.type == SDL_QUIT) quit = true;}}

    
	return 0;
}