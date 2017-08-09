# Support for Windows UWP

Windows UWP support is being done through CMake, and thus depends much on CMake for it to work.
Requires CMake 3.4+ in order to work, and installing the Windows 10.0 UWP SDK in Visual Studio.

Similar to Win32, you can use the Windows makefiles to set up a UWP environment.
This is the recommended approach, since it requires ANGLE to work,
which is downloaded automatically by the makefile.

# What does not work
 - All of the file API, needs rework for file redirection
 - Resources are not being packed at all
