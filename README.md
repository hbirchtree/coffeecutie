# CoffeeCutie
A C++ game engine/rendering engine written in a very C-influenced way.

# Build status

|Travis CI  |Appveyor CI | Codecov.io |
|:----------|:-----------|
[![Build Status](https://travis-ci.org/hbirchtree/coffeecutie.svg?branch=master)](https://travis-ci.org/hbirchtree/coffeecutie)|\
[![Build status](https://ci.appveyor.com/api/projects/status/nt47us32mp6cc0fa/branch/master?svg=true)](https://ci.appveyor.com/project/hbirchtree/coffeecutie/branch/master)|\
[![codecov](https://codecov.io/gh/hbirchtree/coffeecutie/branch/master/graph/badge.svg)](https://codecov.io/gh/hbirchtree/coffeecutie)|

# What is this?
Game engine with platform support for Linux, OS X, Windows, Android

# What is the progress report?
 - Ports:
   - Linux
     - Desktops
     - SteamOS builds are working
     - Nokia N900 (using GCC 6)
     - Raspberry Pi (to some degree, if only DispManX wasn't so terrible)
     - ODROID-XU4 (provided you have a working GLES driver)
     - Supports packaging into self-contained AppImage files
     - Packaging solutions such as Flatpak and Snap kind of work, but not really
   - Windows
     - Win32 is the only one that works properly, it also bakes resources into the executable
     - UWP is underway, but it will probably run on ANGLE (no plans for D3D support)
   - Android (only ARM64 and ARMv7a are truly tested, but it compiles for MIPS\* and x86\_\*!)
   - Mac OS X (compiles for iOS as well, but no guarantee of actually working)
   - Emscripten (barely works)
   - NaCL (barely works)
 - Has OpenGL backends for 3.3 and 4.3, OpenGL ES backends for 2.0 and 3.0

# Platform support tl;dr
 - Linux support is dandy, always maintained
 - Windows is kind of wonky, not all Linux features are present (such as retrieving memory allocations by library in memory)
 - OS X port is pretty good-looking
 - ReactOS support might happen if it supports OpenGL
 - Wrapper code for Python, D-language and C# are in their early phases, currently support creation of windows and GL contexts, as well as clearing said GL context
 - Refrigerator port has yet to be realized, contributors appreciated
 - Toaster edition might follow refrigerator port
 - coffeecutie on a cob is off the table
 - Java port will not happen
