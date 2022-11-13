all:
	c++ -Wall `sdl2-config --cflags` -I imgui/ *.cpp imgui/backends/imgui_impl_sdl.cpp imgui/backends/imgui_impl_sdlrenderer.cpp imgui/imgui*.cpp `sdl2-config --libs` -lGL -o main
debug:
	c++ -g -Wall `sdl2-config --cflags` -I imgui/ *.cpp imgui/backends/imgui_impl_sdl.cpp imgui/backends/imgui_impl_sdlrenderer.cpp imgui/imgui*.cpp `sdl2-config --libs` -lGL -o main