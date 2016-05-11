General dependencies
====================
 - Compiler of choice (but not really on Windows)

     a. Clang 3.5+ (proven and tested!)
     
     b. GCC 4.9+ (eh, as long as it supports C++11 well enough)
     
     c. Visual C++ 2015+ (because Microsoft is slow to catch up on standards)

 - OpenAL development libraries
 - OpenSSL development libraries
 - SDL2 development libraries (min. of 2.0.3, 2.0.4 is preferred)
 - An OpenGL driver (duh)
 - CMake 3.0+ (required for C++11 flags)
 - Ninja-build on Linux and OSX, to help the compile time
 - Some time

        # Tip: use
        cmake ${PROJECT_DIR} -LH
        # to get build options

For the lazy
-----------------------
This will give you a basic build with OpenGL, OpenAL and SDL2 event support:
 1. Make a build directory
 2. Configure cmake with:
    
        cmake ${PROJECT_DIR}
 3. Build it

For the really lazy
------------------------------
Do the above, but adding the following to the cmake commandline:

        -DCOFFEE_BUILD_OPENAL=OFF -DCOFFEE_BUILD_SDL2=OFF -DCOFFEE_BUILD_OPENSSL=OFF

This removes the need for any external dependencies apart from a compiler, but with heavily reduced functionality (command-line only).

Really detailed
--------------------------

 1. Create a build directory
 2. Configure CMakeLists.txt from the root directory in the build directory
 3. Build the application, providing paths to Assimp, GLBinding and others if applicable.
 4. If all goes well, binaries are output to {build directory}/bin, readily linked and ready to go

tl;dr:

        mkdir build && cd build
        cmake ${PROJECT_DIR}
        ninja

Some applications have sample data, sourced from the sample_data directory in the root of the project directory. Some applications will crash without these files.
