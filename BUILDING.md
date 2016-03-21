General dependencies
====================
 - Compiler of choice (but not really on Windows)

     a. Clang (proven and tested!)
     
     b. GCC
     
     c. Visual C++ 2015+ (because Microsoft is slow to catch up on standards)

 - OpenAL development libraries
 - OpenSSL development libraries
 - An OpenGL driver (duh)
 - CMake 3.0+ (required for C++11 flags)
 - Ninja on Linux and OSX, to help the compile time
 - Some time

On Linux...
==============
For the lazy (on Linux)
-----------------------
This will give you a basic build with OpenGL, OpenAL and SDL2 event support:
 1. Make a build directory
 2. Configure cmake with:
    
        cmake ${PROJECT_DIR}
 3. Build it

For the really lazy (on Linux)
------------------------------
Do the above, but adding the following to the cmake commandline:

        -DCOFFEE_BUILD_OPENAL=OFF -DCOFFEE_BUILD_SDL2=OFF -DCOFFEE_BUILD_OPENSSL=OFF

This removes the need for any external dependencies apart from a compiler, but with heavily reduced functionality (command-line only).

Really detailed (on Linux)
--------------------------
Ubuntu packages (for core functionality):

        # Core functionality
        apt install cmake ninja-build build-essential libopenal-dev libsdl2-dev libssl-dev libunwind-dev
        
        # Media extensions, language bindings, GUI tools
        apt install libavcodec-dev libavdevice-dev libavformat-dev libswresample-dev libswscale-dev swig3.0 qt5-default

 1. Create a build directory
 2. Configure CMakeLists.txt from the root directory in the build directory
 3. Build the application, providing paths to Assimp, GLBinding and others if applicable.
 4. If all goes well, binaries are output to {build directory}/bin, readily linked and ready to go

tl;dr:

        mkdir build && cd build
        cmake ${PROJECT_DIR}
        ninja

Some applications have sample data, sourced from the sample_data directory in the root of the project directory. Some applications will crash without these files.

On Windows...
=============

The workflow is as such:
 1. Configure with CMake. Just use the dam* GUI and get over with it. You will have to specify a lot of filepaths since Windows has no automatic library directories or include directories.
 2. Browse to build directory
 3. Either:

    a. Open cmd.exe and execute (assuming MSBuild.exe is in PATH):

        MSBuild.exe Coffee.sln

    b. Open Visual Studio and open Coffee.sln, build from there


**NB**: Note that Windows is not fully supported or tested at all times.

On Mac OS X...
==============
There is no status on this port.

**NB**: Note that Mac OS X is not supported (yet) and might not work at all.
