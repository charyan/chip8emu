#include "headers/frontend.h"
#include "imgui/imgui_memory_editor.h"

#include <SDL2/SDL_ttf.h>
#include <cstring>
#include <string>

#include <stack>

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
    getRoms();
    isDebugEnabled = _isDebugEnabled;
    winW = _winW;
    winH = _winH;
    bufferW = _bufferW;
    bufferH = _bufferH;

    clockFreq = 0.;

    memoryWindow_isOpen = false;
    chip8Window_isOpen = true;
    chip8Window_isPaused = false;

    systemWindow_isOpen = true;

    // Setup SDL
    // (Some versions of SDL before <2.0.10 appears to have performance/stalling issues on a minority of Windows systems,
    // depending on whether SDL_INIT_GAMECONTROLLER is enabled or disabled.. updating to the latest version of SDL is recommended!)
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0)
    {
        printf("Error: %s\n", SDL_GetError());
        exit(1);
    }

    SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
    window = SDL_CreateWindow(_winTitle.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, _winW, _winH, window_flags);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    tex = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_BGRA32, SDL_TEXTUREACCESS_STATIC, _bufferW, _bufferH);
    pixels = new uint32_t[_bufferW * _bufferH];

    // update();

    // if(isDebugEnabled){
    //     TTF_Init();
    //     debugWindow = SDL_CreateWindow("DEBUG", dw-1000, SDL_WINDOWPOS_CENTERED, 1000, 1000, SDL_WINDOW_SHOWN );
    //     debugRenderer = SDL_CreateRenderer(debugWindow, -1, SDL_RENDERER_ACCELERATED);
    //     font = TTF_OpenFont("./IBMPlexMono-Medium.ttf", 24);
    // }

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void)io;
    // io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    // io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    // ImGui::StyleColorsLight();

    // Setup Platform/Renderer backends
    ImGui_ImplSDL2_InitForSDLRenderer(window, renderer);
    ImGui_ImplSDLRenderer_Init(renderer);
}

/**
 * @brief Draw new ImGui frame
 *
 */
void frontend::newImGuiFrame()
{
    // Start the Dear ImGui frame
    ImGui_ImplSDLRenderer_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();
}

/**
 * @brief Draw chip 8 window
 *
 */
void frontend::drawChip8Window()
{
    {
        ImGui::Begin("Chip 8 Window", &chip8Window_isOpen); // Create a window called "Hello, world!" and append into it.
        ImGui::SetWindowPos({20,20}, ImGuiCond_Once);
        ImGui::SetWindowSize({500,500}, ImGuiCond_Once);
        std::string wName = (chip8Window_isPaused) ? "RUN" : "PAUSE";
        if (ImGui::Button(wName.c_str(), {80, 20}))
        {
            chip8Window_isPaused = !chip8Window_isPaused;
        }

        if (!chip8Window_isPaused)
        {
            ImGui::SameLine();
            std::string ts = std::to_string((int)clockFreq) + "Hz";
            ImGui::Text(ts.c_str());
        }

        ImGui::Image(tex, ImVec2(ImGui::GetWindowWidth(), ImGui::GetWindowHeight() - 60));

        ImGui::End();
    }
}

/**
 * @brief Draw the main menu bar
 *
 */
void frontend::drawMainMenuBar()
{
    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("Menu"))
        {
            if (ImGui::BeginMenu("Load ROM"))
            {
                for (int i = 0; i < roms.size(); ++i)
                {
                    if (ImGui::MenuItem(roms[i].c_str()))
                    {
                        clear();
                        app->ch->load(roms[i]);
                    }
                }

                ImGui::EndMenu();
            }
            if (ImGui::MenuItem("Clear System"))
            {
                clear();
                app->ch->reset();
            }

            ImGui::EndMenu();
        }

        // if (ImGui::MenuItem("MenuItem")) {} // You can also use MenuItem() inside a menu bar!
        if (ImGui::BeginMenu("Windows"))
        {
            if (ImGui::MenuItem("Chip 8"))
                chip8Window_isOpen = !chip8Window_isOpen;
            if (ImGui::MenuItem("System"))
                systemWindow_isOpen = !systemWindow_isOpen;
            if (ImGui::MenuItem("Memory"))
                memoryWindow_isOpen = !memoryWindow_isOpen;

            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }
}

/**
 * @brief Draw system window
 *
 */
void frontend::drawSystemWindow()
{
    ImGui::Begin("System", &systemWindow_isOpen); // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)

    ImGui::SetWindowPos({700,20}, ImGuiCond_Once);
    ImGui::SetWindowSize({500,700}, ImGuiCond_Once);

    static ImGuiTableFlags flags = ImGuiTableFlags_SizingStretchSame | ImGuiTableFlags_BordersInner | ImGuiTableFlags_BordersV | ImGuiTableFlags_ContextMenuInBody;

    ImGui::Text("V registers");
    if (ImGui::BeginTable("table1", 4, flags))
    {
        int v = 0;
        for (int row = 0; row < 4; row++)
        {
            ImGui::TableNextRow();
            for (int column = 0; column < 4; column++)
            {
                ImGui::TableSetColumnIndex(column);
                ImGui::Text("V%X", v);
                ImGui::SameLine();
                ImGui::Text("0x%02X", app->ch->V[v]);
                v++;
            }
        }
        ImGui::EndTable();
    }

    ImGui::NewLine();
    if (ImGui::BeginTable("table1", 4, flags))
    {
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("PC");
        ImGui::SameLine();
        ImGui::Text("0x%04X", app->ch->PC);

        
        ImGui::TableSetColumnIndex(1);
        ImGui::Text("I");
        ImGui::SameLine();
        ImGui::Text("0x%04X", app->ch->I);

        
        ImGui::TableSetColumnIndex(2);
        ImGui::Text("delayTimer");
        ImGui::SameLine();
        ImGui::Text("%d", app->ch->delayTimer);

        
        ImGui::TableSetColumnIndex(3);
        ImGui::Text("soundTimer");
        ImGui::SameLine();
        ImGui::Text("%d", app->ch->soundTimer);

        ImGui::EndTable();
    }

    ImGui::NewLine();

    ImGui::InputInt("Clock frequence (Hz)", &clockFreq);


    ImGui::NewLine();

    if(ImGui::CollapsingHeader("Stack")){
        if (ImGui::BeginTable("table1", 4, flags)){
            std::stack stackCopy = app->ch->stack;
            for(int i=0; i<stackCopy.size(); ++i){
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::Text("%d.", i);
                ImGui::TableSetColumnIndex(1);
                ImGui::Text("0x%04X", stackCopy.top());
                stackCopy.pop();
            }

            ImGui::EndTable();
        }
    }


    ImGui::End();
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

    // if(isDebugEnabled){
    //     SDL_DestroyRenderer(debugRenderer);
    //     SDL_DestroyWindow(debugWindow);
    //     TTF_CloseFont(font);
    // }

    // Cleanup
    ImGui_ImplSDLRenderer_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

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
void frontend::draw(uint32_t x, uint32_t y, uint32_t color)
{
    pixels[x + y * bufferW] = color;
}

/**
 * @brief Copy buffer to window
 */
void frontend::update()
{
    newImGuiFrame();

    SDL_UpdateTexture(tex, NULL, pixels, 4 * bufferW);

    // ImGui::ShowDemoWindow();

    if (chip8Window_isOpen)
    {
        drawChip8Window();
    }
    if (systemWindow_isOpen)
    {
        drawSystemWindow();
    }
    if (memoryWindow_isOpen)
    {
        drawMemoryEditor();
    }

    drawMainMenuBar();

    // Rendering
    ImGui::Render();
    SDL_SetRenderDrawColor(renderer, (Uint8)(0), (Uint8)(0), (Uint8)(0), (Uint8)(0));
    SDL_RenderClear(renderer);

    // SDL_RenderCopy(renderer, tex, NULL, NULL);

    ImGui_ImplSDLRenderer_RenderDrawData(ImGui::GetDrawData());
    SDL_RenderPresent(renderer);
}

/**
 * @brief Set all pixels to color (ARGB32)
 * @param color uint32_t 0xAARRGGBB
 */
void frontend::setColor(uint32_t color)
{
    for (int y = 0; y < (int)bufferH; ++y)
    {
        for (int x = 0; x < (int)bufferW; ++x)
        {
            pixels[x + y * bufferW] = color;
        }
    }
    update();
}

/**
 * @brief List all files in path ./roms/
 *
 */
void frontend::getRoms()
{
    for (const auto &entry : std::filesystem::directory_iterator("roms/"))
        roms.push_back(entry.path());
}

void frontend::clear()
{
    for (int i = 0; i < bufferW * bufferH; ++i)
    {
        pixels[i] = 0;
    }
}

void frontend::drawMemoryEditor()
{
    static MemoryEditor mem_edit;
    mem_edit.OptShowAscii = false;
    mem_edit.OptShowDataPreview = false;
    mem_edit.ReadOnly = true;
    mem_edit.OptUpperCaseHex = true;

    mem_edit.HighlightMin = app->ch->PC;
    mem_edit.HighlightMax = app->ch->PC+2;
    mem_edit.HighlightColor = 0xFF0000FF;

    mem_edit.DrawWindow(&memoryWindow_isOpen, "Memory", app->ch->ram, 4096);
}