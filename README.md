# CoffeeCutie
A C++ game engine/rendering engine written in a very C-influenced way.

# Build status

|Platform   |Status      |
|:----------|:-----------|
|Mac        |[![Build Status](https://travis-ci.org/hbirchtree/coffeecutie.svg?branch=master)](https://travis-ci.org/hbirchtree/coffeecutie)|
|Linux      |[![Build Status](https://travis-ci.org/hbirchtree/coffeecutie.svg?branch=master)](https://travis-ci.org/hbirchtree/coffeecutie)|
|Windows    |[![Build status](https://ci.appveyor.com/api/projects/status/t6b9f7q3i6erk3r7/branch/master?svg=true)](https://ci.appveyor.com/project/hbirchtree/coffeecutie/branch/master)|

# What is this?
A game engine comprising a GL renderer (with full OpenGL 4.3 support, 4.5 will be there soon), media playback (thanks to FFMPEG), a platform layer, audio through OpenAL and free choice (as in: no integration) for physics engines. It consists of a custom platform layer, with codepaths for Windows, Linux and Android so far.

# What is the progress report?
 - Ports:
   - Runs on any Linux distro that supports OpenGL (of which there are many)
   - Windows port is in
   - Android port is in
   - OS X port is in
 - Current functionality:
   - Lets you create OpenAL contexts for audio playback and streaming with a relatively simple API (see examples)
   - Abstraction layer for OpenGL 3.3 and 4.3 is in place, heavily inlined code, uses enums for a lot of functions, still opens up for using pure OpenGL functions.
   - Uses SDL2 for window creation, theoretically works on a toaster
   - Some type abstractions for the sake of porting ease (eg. int64 on Android)
   - Tons of platform-abstraction, from files to networking (the latter is still WIP)

# Platform support tl;dr
 - Linux support is dandy, always maintained
 - Windows port is about as good as Linux, some lack in platform layer for non-essential code
 - Android port is all good, although lacking in some aspects (no scratch buffers), it will work well in time
 - OS X kind of works, with some hiccups in setting it up (eg. needing a newer version of Clang)
 - ReactOS support might happen
 - Wrapper code for Python, D-language and C# are in their early phases, currently support creation of windows and GL contexts, as well as clearing said GL context
 - Symbian support might happen in a parallel universe where Symbian supports C++11
 - Maemo crossed my mind months ago, and might happen if it supports C++11
 - Refrigerator port has yet to be realized, contributors appreciated
 - Toaster edition might follow refrigerator port
 - coffeecutie on a cob is off the table
 - Java port will not happen
