# Port details

Raspberry Pi support currently depends on SDL2 for interacting with the hardware window manager. Currently, OpenGL ES 2.0 applications will override the entire screen. There is also potential for crashing the Raspberry Pi.

## Building on Raspberry Pi
Raspberry packages:

        # Essential packages (Ubuntu, Debian)
        apt install libopenal-dev libsdl2-dev libssl-dev libunwind-dev

        # Essential packages (Arch)
        pacman -S gcc libunwind sdl2 libpulse openal libx11 libxext openssl

Arch and others will have to acquire similar packages or build them.
Static builds of SDL2 are recommended for better portability.

Compiling the project should be done through cross-compiling (compiling on-device is suicide)
An included Docker builder will do this task for you, setting up the Raspberry/Broadcom toolchain (found in `builder/raspberry`).

For simple use, `quick-build.sh` has a `raspberry.armel` target which also downloads the Raspberry Pi SDK for you.

*NB:* For now, the Raspberry Pi build has several ABI problems, probably due to bad libraries in `native-library-bundle`. This has not been prioritied.
