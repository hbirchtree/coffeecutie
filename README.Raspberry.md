# Building on Raspberry Pi
Raspberry packages:

        # Essential packages (Ubuntu, Debian)
        apt install libopenal-dev libsdl2-dev libssl-dev libunwind-dev

        # Essential packages (Arch)
        pacman -S gcc libunwind sdl2 libpulse openal libx11 libxext openssl

Arch and others will have to acquire similar packages or build them.
Static builds of SDL2 are recommended for better portability.

Compiling the project should be done through cross-compiling (compiling on-device is suicide)
An included Docker builder will do this task for you, setting up the Raspberry/Broadcom toolchain (found in builder/raspberry).

From there, run CMake with:

        cmake -C${PROJECT_DIR}/cmake/Preload/gnueabihf-arm-raspberry.cmake \
            -DCMAKE_TOOLCHAIN_FILE=${PROJECT_DIR}/cmake/Toolchains/gnueabihf-arm-raspberry.toolchain.cmake \
            -DRASPBERRY_SDK=${RASPBERRY_SYSROOT} \
            ${PROJECT_DIR}

Where RASPBERRY_SDK must contain the basic libraries needed to compile the application. (Preferably using the above provided packages turned into a sysroot)

The output binaries are then ready to run on a Raspberry Pi device or in a QEMU environment.
