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
    c.load("danm8ku.ch8");

    SDL_Event e;
    uint64_t t0 = 0;
    uint64_t t1 = 1000;

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
            if(t1-t0 >= 10){
                c.tick();
                t0 = t1;
            }
            
            t1 = SDL_GetTicks64();
        }
    }

    quit:

    // SDL_Event e; bool quit = false; while (quit == false){while(SDL_PollEvent(&e)){if(e.type == SDL_QUIT) quit = true;}}

    
	return 0;
}