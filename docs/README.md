# Subsystem SDL2

## Installing on Windows

## Installing on Linux

- Debian

   ```
   sudo apt update
   sudo apt install libsdl2-dev libsdl2-ttf-dev libsdl2-image-dev
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
