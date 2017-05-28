# What is this stuff?

These are automated Makefiles which will execute CMake, configure your project and build it, all possibly inside a Docker container.
These are made for simple cross-platform builds, because it's a pain to configure Jenkins for everything.

The scripts may be incorporated into Jenkins in the future to replace the behemoth of 1000 lines in Groovy.

The scripts are also useful for building your subproject with dependencies!

## Explanation of scripts

 - Makefile.multi
   - Builds a project using Docker and CMake with tons of flexibility. This is the basis for most builds.
 - Makefile.android
   - Builds Coffee for all Android architectures. Does not work with subprojects.
   - Creates universal Android APKs
 - Makefile.android-standalone
   - Builds a project for a standalone Android architecture.
   - Works with subprojects when configured correctly.
     Options to set:
       - TARGET : flavor of Android, possible values are:
         - arm64_nougat, armv7a-neon_latest, armv7a-neon_kk, ...
       - AUTO_DIR : directory containing Docker build scripts
       - COFFEE_DIR : directory containing compiled Coffee library, should contain a directory named $TARGET
       - SOURCE_DIR : directory containing a Coffee-derived CMake project
   - Does not create universal Android APKs
 - Makefile.choices
   - Contains configurations ready to compile different configurations, ranging from:
     - Ubuntu
     - Fedora
     - Coverage test
     - Documentation
     - Raspberry Pi
     - Maemo
     - Emscripten
     - NaCL
     - Android (using Makefile.android)
