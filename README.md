# Chip 8 emulator made with SDL2 and IMGUI
![Chip 8 emulator screenshot](https://github.com/charyan/chip8emu/raw/master/chip8emu.png)

## Build instructions
First you will need to [install SDL2](https://wiki.libsdl.org/Installation)
```bash
git clone https://github.com/charyan/chip8emu.git
cd chip8emu/
make all
./main
```
If you encouter the following error : `libSDL2-2.0.so.0: cannot open shared object file`
You need to add the path to the SDL2 lib with `export LD_LIBRARY_PATH="/usr/local/lib"`

## Usage
Place your roms in directory `chip8emu/roms/`. Use `SPACE` to pause the execution of the chip8 executable.

## Keyboard mapping
```
[Chip8]      [Keyboard]
1 2 3 C      1 2 3 4
4 5 6 D  --> Q W E R
7 8 9 E      A S D F
A 0 B F      Z X C V
```
