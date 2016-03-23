Linux details
=============
All functionality can be assumed to be implemented, from file mapping to memory caching. This port is used as an example for all platforms to follow. Any functionality in any case is ported from this to the others when appropriate.

Windows details
===============
Where possible, POSIX-dependent code is used, but Win32-code is to be expected in the platform layer. While mostly relying on the core Win32 functions, some external (pathcch for example) are used.
Apart from these features, the port is quite on par with the Linux port.

Android details
===============
This port is pure sorcery. The SDL2 examples for creating an APK were used as a base in order to make it work in the first place, and anything else is hacked on top. A Docker container is used to contain the mess that is the Android SDK and NDK, while also compiling SDL2 and OpenAL for both armeabi-v7a and arm64-v8a. (These core dependencies are needed for a good build, otherwise there is nothing interesting.)
The compilation process is mostly stock, with an Android NDK toolchain for CMake.
Integration with JNI functions is yet to be implemented, but will likely be done to facilitate more functionality. (Such as launching Android-native intentions)
