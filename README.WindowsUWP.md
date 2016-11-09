# Support for Windows UWP

Windows UWP support is being done through CMake, and thus depends much on CMake for it to work.
Requires CMake 3.4+ in order to work, and installing the Windows 10.0 UWP SDK in Visual Studio.

# What does not work
 - Rendering with OpenGL (duh)
 - All of the file API, needs rework for file redirection
 - Resources are not being packed at all
