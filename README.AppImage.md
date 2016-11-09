# Support for AppImage format

Support has been made for a less strict mode of packaging, but also the FSH-model proposed by the creators.
By default, the less strict mode is used. To enable the standard mode, set the CMake flag APPIMAGE_FOLLOW_STANDARD=ON.

The packaging is done using the script found in "cmake/Packaging/LinuxAppImageBuild.cmake".

Everything works as expected, since no attempt at isolation is being done. Resource filenames should be kept simple in order for AppImage to work correctly.
