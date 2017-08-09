# Building for Maemo 5
The Docker container called hbirch/maemo-builder:v1 (available on Docker Hub) contains a chroot environment running a Maemo 5 SDK installation running on top of QEMU for ARM. (Yes, that is right, GCC for ARM on ARM running on x86)

Apart from this, CMake can be run inside the container with only a toolchain and preload configuration.

## Building SDL2 for Maemo 5
GCC 4.2.x (bundled with Maemo 5) will not work, GCC 4.8.1 will however (and is bundled with the Docker container).
The following configuration options will provide the basics:

        ./configure --disable-video-opengl --disable-video-opengles1 \\
            --disable-audio --disable-threads --disable-timers \\
            --disable-loadso --disable-filesystem --disable-cpuinfo \\
            --disable-file --disable-assembly --disable-atomic \\
            --disable-render --disable-shared
