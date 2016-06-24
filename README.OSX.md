# Port details
Reuses a lot of the POSIX-compliant code from the Linux version, with minor modifications to some functions. (Such as file mapping, where Linux defines its own rules)
Some missing features as of (14-06-16):
 - System information (processor data etc.)

# How to build and deploy for OS X?

On OS X, you will likely have to download Clang 3.8+ along with its C++ library (libc++) and use this. The project requires C++11 and up compatibility which is not found with Apple's older versions of Xcode. (TLS in particular) Once compilation works, .app containers will be generated with resources included. Have fun.

I (the developer) do this:

0. Install Xcode CLI tools (eg. run clang in terminal once)
1. Install Clang, Ninja and CMake through homebrew

          brew install homebrew/versions/llvm38 --with-clang
          brew install ninja cmake
2. Install SDL2 frameworks (SDL's website provides this, the development version)
3. Compile it! You will quickly see the appearance of .app packages in the build/bin directory.

An iOS port is planned, and will likely use the OSX port as its backbone.
