# How to build and deploy for OS X?

On OS X, you will likely have to download Clang 3.8+ along with its C++ library (libc++) and use this. The project requires C++11 and up compatibility which is not found with Apple's older versions of Xcode. (TLS in particular) Once compilation works, .app containers will be generated with resources included. Have fun.
