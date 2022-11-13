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

            if(e.type==SDL_KEYDOWN){
                switch (e.key.keysym.sym)
                {
                    case SDLK_SPACE:
                        fr->chip8Window_isPaused = !fr->chip8Window_isPaused;
                        break;
                    case SDLK_1:
                        ch->keyPressed_1 = true;
                        break;
                    case SDLK_2:
                        ch->keyPressed_2 = true;
                        break;
                    case SDLK_3:
                        ch->keyPressed_3 = true;
                        break;
                    case SDLK_4:
                        ch->keyPressed_c = true;
                        break;
                    case SDLK_q:
                        ch->keyPressed_4 = true;
                        break;
                    case SDLK_w:
                        ch->keyPressed_5 = true;
                        break;
                    case SDLK_e:
                        ch->keyPressed_6 = true;
                        break;
                    case SDLK_r:
                        ch->keyPressed_d = true;
                        break;
                    case SDLK_a:
                        ch->keyPressed_7 = true;
                        break;
                    case SDLK_s:
                        ch->keyPressed_8 = true;
                        break;
                    case SDLK_d:
                        ch->keyPressed_9 = true;
                        break;
                    case SDLK_f:
                        ch->keyPressed_e = true;
                        break;
                    case SDLK_v:
                        ch->keyPressed_f = true;
                        break;
                    case SDLK_z:
                        ch->keyPressed_a = true;
                        break;
                    case SDLK_x:
                        ch->keyPressed_0 = true;
                        break;
                    case SDLK_c:
                        ch->keyPressed_b = true;
                        break;         
                default:
                    break;
                }
            } else if(e.type==SDL_KEYUP) {
                switch (e.key.keysym.sym)
                {
                    case SDLK_1:
                        ch->keyPressed_1 = false;
                        break;
                    case SDLK_2:
                        ch->keyPressed_2 = false;
                        break;
                    case SDLK_3:
                        ch->keyPressed_3 = false;
                        break;
                    case SDLK_4:
                        ch->keyPressed_c = false;
                        break;
                    case SDLK_q:
                        ch->keyPressed_4 = false;
                        break;
                    case SDLK_w:
                        ch->keyPressed_5 = false;
                        break;
                    case SDLK_e:
                        ch->keyPressed_6 = false;
                        break;
                    case SDLK_r:
                        ch->keyPressed_d = false;
                        break;
                    case SDLK_a:
                        ch->keyPressed_7 = false;
                        break;
                    case SDLK_s:
                        ch->keyPressed_8 = false;
                        break;
                    case SDLK_d:
                        ch->keyPressed_9 = false;
                        break;
                    case SDLK_f:
                        ch->keyPressed_e = false;
                        break;
                    case SDLK_v:
                        ch->keyPressed_f = false;
                        break;
                    case SDLK_z:
                        ch->keyPressed_a = false;
                        break;
                    case SDLK_x:
                        ch->keyPressed_0 = false;
                        break;
                    case SDLK_c:
                        ch->keyPressed_b = false;
                        break;         
                default:
                    break;
                }
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
