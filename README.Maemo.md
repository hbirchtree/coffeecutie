# Port details

The Maemo port is only concerned with the Nokia N900 with Maemo 5.

Currently, the Maemo port runs directly on top of EGL and X11, both backends written from scratch. Some X11 commands are specially tailored for the Hildon environment, and might not work if Hildon is not present.

## Building for Maemo

The Docker container called `hbirch/maemo-builder:v1` (available on Docker Hub) contains a chroot environment running a Maemo 5 SDK installation running on top of QEMU for ARM. (Yes, that is right, GCC for ARM on ARM running on x86)

Apart from this, CMake can be run inside the container with only a toolchain and preload configuration.

For now, even though there is a `maemo.armel` target in `quick-build.sh`, it does not compile due to ABI issues.
