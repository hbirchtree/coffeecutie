# CoffeeCutie
<img src="./toolchain/desktop/common/icon.svg" alt="Logo" width="100px"> 
A C++ game engine/rendering engine written in a very C-influenced way.


# What is this?
Game engine with platform support for Linux, OS X, Windows, Android and web

# What is the progress report?
 - Ports:
   - Linux
     - ARMv7a, ARM64, x86\_64
     - Compiles using GCC 15+ across platforms, not distro compiler
     - X11 and Wayland support with SDL2
     - Plain EGL for SBCs
     - Tested on NVIDIA and Intel GPUs
     - Tested on SBCs such as:
       - Banana Pi M2 Berry (Allwinner A40i-based, ARM Mali-400MP GPU)
       - Beaglebone Black (TI Sitara AM335x-based, PowerVR SGX 530 GPU)
       - Orange Pi 5 (RK3588-based, ARM Mali-G610 GPU)
       - Raspberry Pi 3 (BCM2835-based, Broadcom VC4 GPU)
   - Windows
     - Compiled using MinGW, VC++ likely incompatible
     - Tested on NVIDIA and AMD GPUs
   - Android
     - ARMv7a maintained back to Kitkat
     - ARM64 well maintained
     - Tested on Mali and Adreno GPUs
   - macOS
     - Sparsely tested on x86\_64, not on ARM64
     - Limited to whatever the macOS driver gives us
   - Emscripten
     - Mostly tested on Chrome, Firefox likely works
     - Supports Wasm32/Wasm64
     - Persistent storage, cached downloads
     - Experimental GameNetworkingSockets
   - Highly experimental Gamecube version
     - Based on DevkitPPC
     - Uses a bunch of hacks to enable `mmap()`-like access to DVD drive

