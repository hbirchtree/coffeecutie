# Support for Flatpak

The packaging is done using the script found in "cmake/Packaging/LinuxFlatpakBuild.cmake".

Resources and etc. are all good, most libraries are provided by the system.

The current support creates a .flatpak file for distribution.

## Issues with Flatpak

 - OpenGL, a big showstopper, because we cannot test OpenAL without it for some reason:

        libGL error: No matching fbConfigs or visuals found
        libGL error: failed to load driver: swrast

 - Audio support is flaky/non-existent?

Most features work when a compatible OpenGL configuration is present (eg. Mesa, NVIDIA proprietary does not work)

A lot of issues with Flatpak involve the use of GPU drivers, where libGL is not being used by runtimes.
I really hope they get their sh*t together someday.
The rest of the project looks promising, especially with persistent application data out-of-the-box.
