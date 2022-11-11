#include "headers/chip8.h"
#include <fstream>
#include <cstdlib>
#include <ctime>



chip8::chip8(frontend* _F)
{
    F = _F;
    displayH = 32;
    displayW = 64;

    ram = (uint8_t*) malloc(4096);
    display = (bool*) malloc(displayW*displayH);
    V = (uint8_t*) malloc(16);
    PC = 0x200;
    I = 0;

    for(int i=0; i<4096; ++i){ram[i] = 0;}
    for(int i=0; i<(int)(displayW*displayH); ++i){display[i] = 0;}
    for(int i=0; i<16; ++i){V[i] = 0;}

    uint8_t font[5*16] = {
        0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
        0x20, 0x60, 0x20, 0x20, 0x70, // 1
        0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
        0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
        0x90, 0x90, 0xF0, 0x10, 0x10, // 4
        0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
        0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
        0xF0, 0x10, 0x20, 0x40, 0x40, // 7
        0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
        0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
        0xF0, 0x90, 0xF0, 0x90, 0x90, // A
        0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
        0xF0, 0x80, 0x80, 0x80, 0xF0, // C
        0xE0, 0x90, 0x90, 0x90, 0xE0, // D
        0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
        0xF0, 0x80, 0xF0, 0x80, 0x80  // F
    };

    // Load font to ram
    for(int i=0; i<5*16; ++i){
        ram[i] = font[i];
    }
}

chip8::~chip8()
{
    free(ram);
    free(display);
    free(V);
}

/**
 * @brief Get the instruction at PC and execute it
 */
void chip8::tick(){
    F->debug(getDebugText());  

    uint16_t ins = fetch();

    if(lastTenInstructions.size()<10){
        lastTenInstructions.push_back(ins);
    } else {
        for(int i=0; i<9;++i){
            lastTenInstructions[i] = lastTenInstructions[i+1];
        }
        lastTenInstructions[9] = ins;
    }

    decode(ins);
}

void chip8::timerTick(){
    if(delayTimer != 0){
        delayTimer--;
    }

    if(soundTimer != 0){
        soundTimer--;
    }
}


std::string chip8::hexString(uint32_t n, uint32_t width){
    char c[8] = {0};
    sprintf(c, "%X", n);
    std::string t = c;
    
    while(t.length()<width){
        t.insert(std::begin(t), '0');
    }

    return t;
}



/**
 * @brief Get debug text
 * 
 * @return std::string 
 */
std::string chip8::getDebugText(){
    std::string txt;
    txt.append("\n");
    txt.append("PC : 0x" + hexString(PC,3) + " || instruction : 0x" + hexString((ram[PC]<<8)|ram[PC+1], 4) + "\n");
    txt.append("I  : 0x" + hexString(I,3) + "\n");
    txt.append("\n");
    txt.append("=================================================================================\n");
    txt.append("| V0 | V1 | V2 | V3 | V4 | V5 | V6 | V7 | V8 | V9 | VA | VB | VC | VD | VE | VF |\n");
    txt.append("| " + hexString(V[0]  ,2) + " | " + hexString(V[1]  ,2) + " | " + hexString(V[2],2)   + " | "
                    + hexString(V[3]  ,2) + " | " + hexString(V[4]  ,2) + " | " + hexString(V[5],2)   + " | "
                    + hexString(V[6]  ,2) + " | " + hexString(V[7]  ,2) + " | " + hexString(V[8],2)   + " | "
                    + hexString(V[9]  ,2) + " | " + hexString(V[0xA],2) + " | " + hexString(V[0xB],2) + " | "
                    + hexString(V[0xC],2) + " | " + hexString(V[0xD],2) + " | " + hexString(V[0xE],2) + " | "
                    + hexString(V[0xF],2) + " |\n");
    txt.append("=================================================================================\n");
    txt.append("\n");
    txt.append("Stack size : " + std::to_string(stack.size()) + " || Delay timer : " + std::to_string(delayTimer) + " || Sound timer : " + std::to_string(soundTimer) + "\n");

    if(!stack.empty()){
        txt.append("stack.top() : " + hexString(stack.top(), 3) + " || ram[stack.top()] : " + hexString((ram[stack.top()]<<8)|ram[stack.top()+1], 4) + "\n");
    } else {
        txt.append("\n");
    }
    txt.append("\n");
    txt.append("\n");
    txt.append("LAST INSTRUCTIONS: \n");

    for(int i=0; i<(int)lastTenInstructions.size(); ++i){
        txt.append(((i==0 && (int)lastTenInstructions.size()==10) ? "" : " ") +  std::to_string(lastTenInstructions.size()-i) + ". 0x" + hexString(lastTenInstructions[i], 4) + "\n");
    }

    for(int i=0; i<10-(int)lastTenInstructions.size(); ++i){
        txt.append("\n");
    }

    txt.append("\n");
    txt.append("\n");
    txt.append("\n");
    txt.append("\n");


    return txt;
}

/**
 * @brief Fetch the 2bytes instruction at PC
 * @return uint16_t instruction
 */
uint16_t chip8::fetch(){
    uint16_t ins = ram[PC];
    ins = ins << 8;
    ins = ins | ram[PC+1];
    PC += 2;
    return ins;
}

/**
 * @brief Decode and execute an instruction
 * @param _instruct instruction
 */
void chip8::decode(uint16_t _instruct){
    uint8_t  X   = (_instruct&0x0F00)>>8;
    uint8_t  Y   = (_instruct&0x00F0)>>4;
    uint8_t  N   = (_instruct&0x000F);
    uint8_t  NN  = (_instruct&0x00FF);
    uint16_t NNN = (_instruct&0x0FFF);

    // printf(":%04X\n", _instruct);
    // printf("X : %X\n", X);
    // printf("Y : %X\n", Y);
    // printf("N : %X\n", N);
    // printf("NN : %02X\n", NN);
    // printf("NNN : %03X\n", NNN);

    switch (_instruct)
    {
    // 00E0 (clear screen)
    case 0x00E0:
        for(int i=0; i<(int)(displayH*displayW); ++i){
            display[i] = false;
        }
        F->setColor(0);
        break;
    // 00EE Return from subroutine
    case 0x00EE:
        PC = stack.top();
        stack.pop();
        
        break;
    // 1NNN (jump)
    case 0x1000 ... 0x1FFF:
        PC = NNN;
        break;
    // 2NNN Call subroutine
    case 0x2000 ... 0x2FFF:
        stack.push(PC);
        PC = NNN;
        break;
    // 3XNN Skip if VX equal NN
    case 0x3000 ... 0x3FFF:
        if(V[X] == NN){
            PC += 2;
        }
        break;
    // 4XNN Skip if VX not equal NN
    case 0x4000 ... 0x4FFF:
        if(V[X] != NN){
            PC += 2;
        }
        break;
    // 5XY0 Skip if VX equal VY
    case 0x5000 ... 0x5FF0:
        if(V[X] == V[Y]){
            PC += 2;
        }
        break;
    // 6XNN (set register VX)   
    case 0x6000 ... 0x6FFF:
        V[X] = NN;
        break;
    
    // 7XNN (add value to register VX)
    case 0x7000 ... 0x7FFF:
        V[X] += NN;
        break;
    
    // 8??? Logical and arithmetic instructions
    case 0x8000 ... 0x8FFF:
        {

            uint8_t type = _instruct&0x000F;

            switch (type)
            {
            // 8XY0 Set VX to VY
            case 0x0:
                V[X] = V[Y];
                break;
            // 8XY1 Binary OR
            case 0x1:
                V[X] = V[X] | V[Y];
                break;
            // 8XY2 Binary AND
            case 0x2:
                V[X] = V[X] & V[Y];
                break;
            // 8XY3 Logical XOR
            case 0x3:
                V[X] = V[X] ^ V[Y];
                break;
            // 8XY4 Add
            case 0x4:
                V[X] += V[Y];
                V[0x0F] = (V[X]+V[Y]>255) ? 1 : 0;
                break;
            // 8XY5 Substract VX-VY
            case 0x5:
                V[X] = V[X] - V[Y];
                break;
            // 8XY6 Shift right
            case 0x6:
                V[X] = V[Y];
                V[0x0F] = V[X] & 0x1;
                V[X] = V[X] >> 1;
                break;
            // 8XY7 Substract
            case 0x7:
                V[X] = V[Y] - V[X];
                break;
            // 8XYE Shift left
            case 0xE:
                V[X] = V[Y];
                V[0x0F] = (V[X] & 0b10000000)>>7;
                V[X] = V[X] << 1;
                break;
            
            default:
                break;
            }
        }
        break;

    // 9XY0 Skip if VX not equal VY
    case 0x9000 ... 0x9FF0:
        if(V[X] != V[Y]){
            PC += 2;
        }
        break;
    // ANNN (set index register I)
    case 0xA000 ... 0xAFFF:
        I = NNN;
        break;

    // BNNN Jump with offset
    case 0xB000 ... 0XBFFF:
        PC = NNN+V[0];
        break;

    // CXNN Random
    case 0xC000 ... 0xCFFF:
        {
            std::srand(std::time(nullptr));
            int random = std::rand();
            V[X] = NN & random;
        }
        break;
    
    // DXYN (display/draw)
    case 0xD000 ... 0XDFFF:
        {
            int x = V[X]%64;
            int y = V[Y]%32;
            V[0x0F] = 0;

            // For each row of the sprite
            for(int i=0; i<N; ++i){
                // For each bit of the Nth line of the sprite
                uint8_t bits = ram[I+i];
                // printf("%02X\n", bits);

                for(int j=0; j<8; ++j){
                    uint32_t lastBit = (bits>>j)&0x01;
                    
                    F->draw(x+(8-j), y+i, (lastBit == 1) ? 0xFFFFFF : 0x0);
                    
                }
            }
            F->update();
        }
        break;
    
    case 0xE000 ... 0xEFFF:
        {
            SDL_Event e;
            SDL_PollEvent(&e);

            /**
             * LAYOUT
             * 1 2 3 C
             * 4 5 6 D
             * 7 8 9 E
             * A 0 B F
             * 
             * MAPPED TO KEYBOARD
             * 1 2 3 4
             * Q W E R 
             * A S D F 
             * Z X C V
             */
            
            if(NN == 0x9E){
                // EX9E Skip if key down
                if(e.type == SDL_KEYDOWN){    
                    switch (V[X])
                    {
                    case 0x00:
                        if(e.key.keysym.sym == SDLK_x){
                            PC += 2;
                        }
                        break;
                    case 0x01:
                        if(e.key.keysym.sym == SDLK_1){
                            PC += 2;
                        }
                        break;
                    case 0x02:
                        if(e.key.keysym.sym == SDLK_2){
                            PC += 2;
                        }
                        break;
                    case 0x03:
                        if(e.key.keysym.sym == SDLK_3){
                            PC += 2;
                        }
                        break;
                    case 0x04:
                        if(e.key.keysym.sym == SDLK_q){
                            PC += 2;
                        }
                        break;
                    case 0x05:
                        if(e.key.keysym.sym == SDLK_w){
                            PC += 2;
                        }
                        break;
                    case 0x06:
                        if(e.key.keysym.sym == SDLK_e){
                            PC += 2;
                        }
                        break;
                    case 0x07:
                        if(e.key.keysym.sym == SDLK_a){
                            PC += 2;
                        }
                        break;
                    case 0x08:
                        if(e.key.keysym.sym == SDLK_s){
                            PC += 2;
                        }
                        break;
                    case 0x09:
                        if(e.key.keysym.sym == SDLK_d){
                            PC += 2;
                        }
                        break;
                    case 0x0a:
                        if(e.key.keysym.sym == SDLK_z){
                            PC += 2;
                        }
                        break;
                    case 0x0b:
                        if(e.key.keysym.sym == SDLK_c){
                            PC += 2;
                        }
                        break;
                    case 0x0c:
                        if(e.key.keysym.sym == SDLK_4){
                            PC += 2;
                        }
                        break;
                    case 0x0d:
                        if(e.key.keysym.sym == SDLK_r){
                            PC += 2;
                        }
                        break;
                    case 0x0e:
                        if(e.key.keysym.sym == SDLK_f){
                            PC += 2;
                        }
                        break;
                    case 0x0f:
                        if(e.key.keysym.sym == SDLK_v){
                            PC += 2;
                        }
                        break;
                    default:
                        break;
                    }
                }

            } else if(NN == (uint8_t)0XA1){
                // EXA1 Skip if key up
                if(e.type == SDL_KEYUP){    
                    switch (V[X])
                    {
                    case 0x00:
                        if(e.key.keysym.sym == SDLK_x){
                            PC += 2;
                        }
                        break;
                    case 0x01:
                        if(e.key.keysym.sym == SDLK_1){
                            PC += 2;
                        }
                        break;
                    case 0x02:
                        if(e.key.keysym.sym == SDLK_2){
                            PC += 2;
                        }
                        break;
                    case 0x03:
                        if(e.key.keysym.sym == SDLK_3){
                            PC += 2;
                        }
                        break;
                    case 0x04:
                        if(e.key.keysym.sym == SDLK_q){
                            PC += 2;
                        }
                        break;
                    case 0x05:
                        if(e.key.keysym.sym == SDLK_w){
                            PC += 2;
                        }
                        break;
                    case 0x06:
                        if(e.key.keysym.sym == SDLK_e){
                            PC += 2;
                        }
                        break;
                    case 0x07:
                        if(e.key.keysym.sym == SDLK_a){
                            PC += 2;
                        }
                        break;
                    case 0x08:
                        if(e.key.keysym.sym == SDLK_s){
                            PC += 2;
                        }
                        break;
                    case 0x09:
                        if(e.key.keysym.sym == SDLK_d){
                            PC += 2;
                        }
                        break;
                    case 0x0a:
                        if(e.key.keysym.sym == SDLK_z){
                            PC += 2;
                        }
                        break;
                    case 0x0b:
                        if(e.key.keysym.sym == SDLK_c){
                            PC += 2;
                        }
                        break;
                    case 0x0c:
                        if(e.key.keysym.sym == SDLK_4){
                            PC += 2;
                        }
                        break;
                    case 0x0d:
                        if(e.key.keysym.sym == SDLK_r){
                            PC += 2;
                        }
                        break;
                    case 0x0e:
                        if(e.key.keysym.sym == SDLK_f){
                            PC += 2;
                        }
                        break;
                    case 0x0f:
                        if(e.key.keysym.sym == SDLK_v){
                            PC += 2;
                        }
                        break;
                    default:
                        break;
                    }

                }
            }

            

            
        }
        break;

    case 0xF000 ... 0XFFFF:
        switch (NN)
        {
        // FX07 Timer
        case 0x07:
            V[X] = delayTimer;
            break;
        
        // FX15 Timer
        case 0x15:
            delayTimer = V[X];
            break;

        // FX18 Timer
        case 0x18:
            soundTimer = V[X];
            break;

        // FX1E Add to index
        case 0x1E:
            I += V[X];
            if(I > 0xFFF){
                V[0xF] = 1;
            }
            break;

        // FX0A Get key
        case 0x0A:
            {
                SDL_Event e;
                SDL_PollEvent(&e);
                if(e.type == SDL_KEYDOWN){
                    switch (e.key.keysym.sym)
                    {
                    case SDLK_1:
                        V[X] = 0x01;
                        break;
                    case SDLK_2:
                        V[X] = 0x02;
                        break;
                    case SDLK_3:
                        V[X] = 0x03;
                        break;
                    case SDLK_4:
                        V[X] = 0x0C;
                        break;
                    case SDLK_q:
                        V[X] = 0x04;
                        break;
                    case SDLK_w:
                        V[X] = 0x05;
                        break;
                    case SDLK_e:
                        V[X] = 0x06;
                        break;
                    case SDLK_r:
                        V[X] = 0x0D;
                        break;
                    case SDLK_a:
                        V[X] = 0x07;
                        break;
                    case SDLK_s:
                        V[X] = 0x08;
                        break;
                    case SDLK_d:
                        V[X] = 0x09;
                        break;
                    case SDLK_f:
                        V[X] = 0x0E;
                        break;
                    case SDLK_v:
                        V[X] = 0x0F;
                        break;
                    case SDLK_z:
                        V[X] = 0x0A;
                        break;
                    case SDLK_x:
                        V[X] = 0x00;
                        break;
                    case SDLK_c:
                        V[X] = 0x0B;
                        break;
                    
                    default:
                        break;
                    }
                } else {
                    PC -= 2;
                }
            }
            break;

        // FX29 Font character
        case 0x29:
            I = 0x00;
            break;

        // FX33 Binary-coded decimal conversion
        case 0x33:
            {
                
                std::string s = std::to_string(V[X]);
                while(s.length()<3){s="0"+s;}

                std::cout << std::to_string(V[X]) << std::endl;
                printf("t[0] %d\n", s[0]-'0');
                printf("t[1] %d\n", s[1]-'0');
                printf("t[2] %d\n", s[2]-'0');

                ram[I+2] = s[2]-'0';
                ram[I+1] = s[1]-'0';
                ram[I] = s[0]-'0';
            }
            break;


        // FX55 Store to memory
        case 0x55:    
            for(int i=0; i<=X; ++i){
                ram[I+i] = V[i];
            }
            break;

        // FX65 Load from memory
        case 0x65:
            if(V[X]==0){
                V[0] = ram[I];
            } else {
                for(int i=0; i<X; ++i){
                    V[i] = ram[I+i];
                }
            }
            break;

        default:
            printf("UNKNOWN INSTRUCTION %04X\n", _instruct);
            break;
        }
        break;


    default:
        break;
    }
}

/**
 * @brief Load file to ram
 * 
 * @param filename 
 */
void chip8::load(std::string filename){
    std::ifstream File;
    File.open(filename);

    for(int i=0;;++i){
        int byte = File.get();
        if(byte == -1) break;
        // printf("%02X ", byte);
        ram[0x200+i] = byte;
    }

    File.close();
}
