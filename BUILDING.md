# General dependencies

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

Provided is a wide array of Makefiles which will automatically download dependencies and build the project.

## Using the Makefiles on *nix

Navigate to a directory outside the source tree, and do as follows:

 1. Install dependencies (for OSX only):
    Refer to README.Linux.md for further details on Linux.

        TRAVIS_OS_NAME=osx $SOURCE_DIR/tools/ci//travis-deps.sh

 2. Build it

	    CONFIGURATION=*Debug|Release*
        GITHUB_TOKEN=*used if you are downloading dependencies*

        $SOURCE_DIR/quick-build.sh [TARGET]

Where `TARGET` is one of the targets found in the Makefile.
This process will download SDKs for most platforms, contained inside Docker containers.

To list available targets:

        $SOURCE_DIR/quick-build.sh list

By default, this method uses Docker containers on Linux (this is not necessary for OS X/iOS targets). In order to generate a native environment, the following environment variable may be defined:

        BUILD_MODE=bare

This changes the runtime of the build to become the bare-metal system. Android builds will require ANDROID\_SDK and ANDROID\_NDK to be defined, and Emscripten is untested. Systems with special toolchains (eg. Fedora, Ubuntu, SteamOS) will not work with this.

## Using the Makefiles on Windows
Navigate to a directory outside the source tree, and do as follows in PowerShell:

    $env:GITHUB_TOKEN = *for downloading engine dependencies, not necessary for engine itself*
    $env:CONFIGURATION = *Debug|Release*

    $SOURCE_DIR\\quick-build.ps1 [TARGET]

Where the following targets are valid:

 - win32.amd64
 - win32.x86
 - uwp.amd64
 - uwp.arm

This process creates a development environment suitable for Visual Studio, as there is no containerization on Windows.

# Development environments
## For the lazy
This will give you a basic build with OpenGL, OpenAL and SDL2 event support:
 1. Make a build directory
 2. Configure cmake with:
    
        cmake ${PROJECT_DIR}
 3. Build it

## For the really lazy
Do the above, but adding the following to the cmake commandline:

        -DCOFFEE_BUILD_OPENAL=OFF -DCOFFEE_BUILD_SDL2=OFF -DCOFFEE_BUILD_OPENSSL=OFF

This removes the need for any external dependencies apart from a compiler, but with heavily reduced functionality (command-line only).

## Really detailed

 1. Create a build directory
 2. Configure CMakeLists.txt from the root directory in the build directory
 3. Build the application, providing paths to Assimp, GLBinding and others if applicable.
 4. If all goes well, binaries are output to {build directory}/bin, readily linked and ready to go

tl;dr:

        mkdir build && cd build
        cmake ${PROJECT_DIR} \
            -DCMAKE_TOOLCHAIN_FILE=$PROJECT_DIR/cmake/Toolchains/...
            -C$PROJECT_DIR/cmake/Preload/...
        ninja

Some applications have sample data, sourced from the sample\_data directory in the root of the project directory. Some applications will crash without these files.
