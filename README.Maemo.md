# Building for Maemo 5
The Docker container called `hbirch/maemo-builder:v1` (available on Docker Hub) contains a chroot environment running a Maemo 5 SDK installation running on top of QEMU for ARM. (Yes, that is right, GCC for ARM on ARM running on x86)

Apart from this, CMake can be run inside the container with only a toolchain and preload configuration.

For now, even though there is a `maemo.armel` target in `quick-build.sh`, it does not compile due to ABI issues.
