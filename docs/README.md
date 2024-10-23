# Subsystem SDL2

## Installing on Windows
This project uses a Makefile to build the library. To execute the Makefile on Windows, you must install a version of GNU make,
which is available in toolkits such as MSYS2, Cygwin, Git Bash, MinGW, or through the Windows Subsystem for Linux (WSL).

- MSYS2

Before installing the packages, make sure you're running the MinGW 64-bit shell.

   ```
   pacman -S mingw-w64-x86_64-gcc
   pacman -S mingw-w64-x86_64-SDL2
   pacman -S mingw-w64-x86_64-SDL2_ttf
   pacman -S mingw-w64-x86_64-SDL2_image
   pacman -S make
   git clone https://github.com/dev-harbour/subsdl.git
   ```
- To build the static library, execute:

   ```
   make
   ```

- To build and run samples and test code, navigate to the tests directory and execute:

   ```
   cd tests
   make
   ```
   or single build:
   ```
   gcc window.c -o window -I ../include -L ../lib -lmingw32 -lsubsdl -lSDL2main -lSDL2 -lSDL2_ttf -lSDL2_image -lm
   ```
## Installing on Linux

- Debian

   ```
   sudo apt update
   sudo apt install libsdl2-dev libsdl2-ttf-dev libsdl2-image-dev
   git clone https://github.com/dev-harbour/subsdl.git
   ```
- To build the static library, execute:

   ```
   make
   ```

- To build and run samples and test code, navigate to the examples directory and execute:

   ```
   cd tests
   make
   ```
   or single build:
   ```
   gcc window.c -o window -I ../include -L ../lib -lsubsdl -lSDL2 -lSDL2_ttf -lSDL2_image -lm
   ```
---
