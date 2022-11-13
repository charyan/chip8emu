#include "headers/Application.h"

Application::Application()
{
    frontend f = frontend("Chip 8 emu", 1280, 720, 64, 32, true);
    fr = &f;
    chip8 c = chip8();
    ch = &c;
    ch->app = this;
    fr->app = this;

    fr->setColor(0);

    fr->clockFreq = 100; // clock per second

    SDL_Event e;

    uint64_t tickTimer = 0;
    uint64_t lastTime = 1000;

    // Used for delayTimer and soundTimer
    uint64_t timerTimer = 0;

    bool done = false;
    fr->chip8Window_isPaused = true;

    while(!done){
        
        while(SDL_PollEvent(&e)){
            ImGui_ImplSDL2_ProcessEvent(&e);
            if (e.type == SDL_QUIT)
                done = true;
            if (e.type == SDL_WINDOWEVENT && e.window.event == SDL_WINDOWEVENT_CLOSE)
                done = true;

            if(e.type==SDL_KEYDOWN && e.key.keysym.sym == SDLK_SPACE){
                fr->chip8Window_isPaused = !fr->chip8Window_isPaused;
            }
        }
        
        if(!fr->chip8Window_isPaused){
            if(lastTime-timerTimer >= (1000/60)){
                ch->timerTick();
                timerTimer = lastTime;
            }

            if(lastTime-tickTimer >= 1000/fr->clockFreq){
                ch->tick();
                tickTimer = lastTime;
            }
            
            lastTime = SDL_GetTicks64();
        }
        
        fr->update();
    }
}

Application::~Application()
{

}
