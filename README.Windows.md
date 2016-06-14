# How to build and deploy for Windows?

You *will* need Visual Studio 2015 in order for compilation to work. (We need full C++11 support, not what VS2013 provides) Compiling 32-bit will not be supported officially; if that breaks, nobody cares.

No successful attempts have been made with Cygwin, MinGW-W64 and MSYS2 so far. (Please report back if it does work!)

The workflow is as such:
 1. Configure with CMake. Just use the dam* GUI and get over with it. You will have to specify a lot of filepaths since Windows has no automatic library directories or include directories.
 2. Browse to build directory
 3. Either:

    a. Open cmd.exe and execute (assuming MSBuild.exe is in PATH):

        MSBuild.exe Coffee.sln

    b. Open Visual Studio and open Coffee.sln, build from there


**NB**: Note that Windows is not fully supported or tested at all times.

# Platform implementation
Uses Win32 where Microsoft won't let us use POSIX. Reuses POSIX functions where possible, although this is not always the case.

UWP
---
No.
