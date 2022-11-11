#include <iostream>

#include "headers/chip8.h"
#include "headers/frontend.h"

#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdbool.h>

/**
 * @todo soundTimer ?
 *       delayTimer ?
 */

const int C8_WIDTH = 64;
const int C8_HEIGHT = 32;

const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;

int main(int argc, char* args[])
{
    frontend fr = frontend("chip8emu", WINDOW_WIDTH, WINDOW_HEIGHT, C8_WIDTH, C8_HEIGHT, true);
	chip8 c = chip8(&fr);
    
    fr.setColor(0);
   
    fr.update();
    c.load("octojam2title.ch8");

    SDL_Event e;
    uint64_t tickTime = 0;
    uint64_t lastTime = 1000;

    // Time used for timers in ms
    uint64_t timerTime = 0;

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
            if(lastTime-timerTime >= (1000/60)){
                c.timerTick();
                timerTime = lastTime;
            }

            if(lastTime-tickTime >= 1){
                c.tick();
                tickTime = lastTime;
            }
            
            lastTime = SDL_GetTicks64();
        }
    }

    quit:

    // SDL_Event e; bool quit = false; while (quit == false){while(SDL_PollEvent(&e)){if(e.type == SDL_QUIT) quit = true;}}

    
	return 0;
}