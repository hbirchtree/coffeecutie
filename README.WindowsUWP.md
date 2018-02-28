# Support for Windows UWP

This build only works with Visual Studio 2017, due to requiring partial C++17 support (used for C++/WinRT).
Windows UWP support is being done through CMake, and thus depends much on CMake for it to work. Version 3.4 of CMake is a minimum (shouldn't be a problem with Chocolatey).

Similar to Win32, you can use the Windows makefiles to set up a UWP environment.
This is the recommended approach, since it requires ANGLE to work,
which is downloaded automatically by the makefile.

# What does not work
 - All of the file API, accessing assets seems impossible. We might just opt for VirtFS all the way.
 - Resources are presumedly not packed correctly
