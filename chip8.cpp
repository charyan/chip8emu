#include "headers/chip8.h"
#include <fstream>

chip8::chip8()
{
    keyPressed_0 = false;
    keyPressed_1 = false;
    keyPressed_2 = false;
    keyPressed_3 = false;
    keyPressed_4 = false;
    keyPressed_5 = false;
    keyPressed_6 = false;
    keyPressed_7 = false;
    keyPressed_8 = false;
    keyPressed_9 = false;
    keyPressed_a = false;
    keyPressed_b = false;
    keyPressed_c = false;
    keyPressed_d = false;
    keyPressed_e = false;
    keyPressed_f = false;

    displayH = 32;
    displayW = 64;

    delayTimer = 0;
    soundTimer = 0;

    ram = (uint8_t*) malloc(4096);
    display = (bool*) malloc(displayW*displayH);
    V = (uint8_t*) malloc(16);
    
    reset();
}

chip8::~chip8()
{
    free(ram);
    free(display);
    free(V);
}

/**
 * @brief Reset PC, I, ram and display
 * 
 */
void chip8::reset(){
    PC = 0x200;
    I = 0;

    while(stack.size()>0){stack.pop();}

    for(int i=0; i<4096; ++i){ram[i] = 0;}
    for(int i=0; i<(int)(displayW*displayH); ++i){display[i] = false;}
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

/**
 * @brief Get the instruction at PC and execute it
 */
void chip8::tick(){
    uint16_t ins = fetch();
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
 * @brief Fetch the 2bytes instruction at PC
 * @return uint16_t instruction
 */
uint16_t chip8::fetch(){
    uint16_t ins = ram[PC];
    ins = (ins << 8);
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
        for(int i=0; i < displayH * displayW; ++i){
            display[i] = false;
        }
        app->fr->setColor(0);
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
    case 0x5000 ... 0x5FFF:
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
                V[X] |= V[Y];
                break;
            // 8XY2 Binary AND
            case 0x2:
                V[X] &= V[Y];
                break;
            // 8XY3 Logical XOR
            case 0x3:
                V[X] ^= V[Y];
                break;
            // 8XY4 Add
            case 0x4:
                V[0x0F] = (V[X]+V[Y]>255) ? 1 : 0;
                V[X] += V[Y];
                break;
            // 8XY5 Substract VX-VY
            case 0x5:
                V[0x0F] = (V[Y]>V[X]) ? 0 : 1;
                V[X] -= V[Y];
                break;
            // 8XY6 Shift right
            case 0x6:
                V[0x0F] = V[Y] & 0x1;
                V[X] = V[Y] >> 1;
                break;
            // 8XY7 Substract
            case 0x7:
                V[0x0F] = (V[X]>V[Y]) ? 0 : 1;
                V[X] = V[Y] - V[X];
                break;
            // 8XYE Shift left
            case 0xE:
                V[0x0F] = V[Y] >> 7;
                V[X] = V[Y] << 1;
                break;
            
            default:
                printf("UNKNOWN INSTRUCTION %04X\n", _instruct);
                break;
            }
        }
        break;

    // 9XY0 Skip if VX not equal VY
    case 0x9000 ... 0x9FFF:
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
            uint8_t random = std::rand()%256;
            V[X] = NN & random;
        }
        break;
    
    // DXYN (display/draw)
    case 0xD000 ... 0XDFFF:
        {
            int x = V[X] % displayW;
            int y = V[Y] % displayH;
            V[0x0F] = 0;

            // For each row of the sprite
            for(int i=0; i<N; ++i){
                if(y + i >= displayH) {
                    continue; // Ignores pixels outside of screen
                }
                    
                // For each bit of the Nth line of the sprite
                uint8_t bits = ram[I+i];

                for(int j=0; j<8; ++j){
                    if(x+j >= displayW ) {
                        continue;
                    }

                    bool pixel = ((bits>>(7-j) & 0x01));

                    int position = (x + j) + (y + i) * displayW;

                    if(pixel && display[position]) {
                        V[0x0F] = 1;
                    }

                    display[position] ^= pixel;
                }
            }

            for(int r_x = 0; r_x < displayW; r_x++) {
                for(int r_y = 0; r_y < displayH; r_y++) {
                    app->fr->draw(r_x, r_y, display[r_x + r_y * displayW] ? 0XFFFFFF : 0x0);
                }
            }

           
            app->fr->update();
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
                switch (V[X])
                    {
                    case 0x00:
                        if(keyPressed_0){PC += 2;}
                        break;
                    case 0x01:
                        if(keyPressed_1){PC += 2;}
                        break;
                    case 0x02:
                        if(keyPressed_2){PC += 2;}
                        break;
                    case 0x03:
                        if(keyPressed_3){PC += 2;}
                        break;
                    case 0x04:
                        if(keyPressed_4){PC += 2;}
                        break;
                    case 0x05:
                        if(keyPressed_5){PC += 2;}
                        break;
                    case 0x06:
                        if(keyPressed_6){PC += 2;}
                        break;
                    case 0x07:
                        if(keyPressed_7){PC += 2;}
                        break;
                    case 0x08:
                        if(keyPressed_8){PC += 2;}
                        break;
                    case 0x09:
                        if(keyPressed_9){PC += 2;}
                        break;
                    case 0x0a:
                        if(keyPressed_a){PC += 2;}
                        break;
                    case 0x0b:
                        if(keyPressed_b){PC += 2;}
                        break;
                    case 0x0c:
                        if(keyPressed_c){PC += 2;}
                        break;
                    case 0x0d:
                        if(keyPressed_d){PC += 2;}
                        break;
                    case 0x0e:
                        if(keyPressed_e){PC += 2;}
                        break;
                    case 0x0f:
                        if(keyPressed_f){PC += 2;}
                        break;
                    default:
                        break;
                    }

            } else if(NN == 0xA1){
                // EXA1 Skip if key up
                switch (V[X])
                    {
                    case 0x00:
                        if(!keyPressed_0){PC += 2;}
                        break;
                    case 0x01:
                        if(!keyPressed_1){PC += 2;}
                        break;
                    case 0x02:
                        if(!keyPressed_2){PC += 2;}
                        break;
                    case 0x03:
                        if(!keyPressed_3){PC += 2;}
                        break;
                    case 0x04:
                        if(!keyPressed_4){PC += 2;}
                        break;
                    case 0x05:
                        if(!keyPressed_5){PC += 2;}
                        break;
                    case 0x06:
                        if(!keyPressed_6){PC += 2;}
                        break;
                    case 0x07:
                        if(!keyPressed_7){PC += 2;}
                        break;
                    case 0x08:
                        if(!keyPressed_8){PC += 2;}
                        break;
                    case 0x09:
                        if(!keyPressed_9){PC += 2;}
                        break;
                    case 0x0a:
                        if(!keyPressed_a){PC += 2;}
                        break;
                    case 0x0b:
                        if(!keyPressed_b){PC += 2;}
                        break;
                    case 0x0c:
                        if(!keyPressed_c){PC += 2;}
                        break;
                    case 0x0d:
                        if(!keyPressed_d){PC += 2;}
                        break;
                    case 0x0e:
                        if(!keyPressed_e){PC += 2;}
                        break;
                    case 0x0f:
                        if(!keyPressed_f){PC += 2;}
                        break;
                    default:
                        break;
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
            break;

        // FX0A Get key
        case 0x0A:
            {
                if(keyPressed_0){V[X] = 0x00;}
                else if(keyPressed_1){V[X] = 0x01;}
                else if(keyPressed_2){V[X] = 0x02;}
                else if(keyPressed_3){V[X] = 0x03;}
                else if(keyPressed_4){V[X] = 0x04;}
                else if(keyPressed_5){V[X] = 0x05;}
                else if(keyPressed_6){V[X] = 0x06;}
                else if(keyPressed_7){V[X] = 0x07;}
                else if(keyPressed_8){V[X] = 0x08;}
                else if(keyPressed_9){V[X] = 0x09;}
                else if(keyPressed_a){V[X] = 0x0a;}
                else if(keyPressed_b){V[X] = 0x0b;}
                else if(keyPressed_c){V[X] = 0x0c;}
                else if(keyPressed_d){V[X] = 0x0d;}
                else if(keyPressed_e){V[X] = 0x0e;}
                else if(keyPressed_f){V[X] = 0x0f;}
                else {PC -= 2;}
            }
            break;

        // FX29 Font character
        case 0x29:
            I = V[X] * 5;
            break;

        // FX33 Binary-coded decimal conversion
        case 0x33:
            {
                ram[I + 0] = V[X] / 100;
                ram[I + 1] = (V[X] / 10) % 10;
                ram[I + 2] = V[X] % 10;
            }
            break;


        // FX55 Store to memory
        case 0x55:    
            for(int i=0; i<=X; ++i){
                ram[I+i] = V[i];
            }
            
            I = I + X + 1; // Not done on super chip 8

            break;

        // FX65 Load from memory
        case 0x65:
            for(int i=0; i<=X; ++i){
                V[i] = ram[I+i];
            }

            I = I + X + 1; // Not done on super chip 8

            break;

        default:
            printf("UNKNOWN INSTRUCTION %04X\n", _instruct);
            break;
        }
        break;


    default:
        printf("UNKNOWN INSTRUCTION %04X\n", _instruct);
        break;
    }

}

/**
 * @brief Load file to ram
 * 
 * @param filename 
 */
void chip8::load(std::string filename){
    reset();
    
    // Load file to ram
    std::ifstream File;
    File.open(filename);

    for(int i=0;i + 0x200 < 4096;++i){
        int byte = File.get();
        if(byte == -1) break;
        // printf("%02X ", byte);
        ram[0x200+i] = byte;
    }

    File.close();
}
