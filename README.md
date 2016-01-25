# CoffeeCutie
A C++ game engine/rendering engine written in a very C-influenced way.

# What is this?
A game engine comprising a GL renderer (with full OpenGL 4.3 support, 4.5 will be there soon), media playback (thanks to FFMPEG), a platform layer, audio through OpenAL and free choice (as in: no integration) for physics engines. It consists of a custom platform layer, with codepaths for Windows, Linux and Android so far.

# What is the progress report?
 - Ports:
   - Runs on any Linux distro that supports OpenGL (of which there are many), Dockerfile is provided with all required dependencies in "tools/ubuntu"
   - Unstable/not working on Windows on MSVC builds. Still in progress.
   - Android is not in any workable condition, but does compile!
 - Current functionality:
   - Lets you create OpenAL contexts for audio playback and streaming with a relatively simple API (see examples)
   - Abstraction layer for OpenGL 3.3 and 4.3 is in place, heavily inlined code, uses enums for a lot of functions, still opens up for using pure OpenGL functions.
   - Uses SDL2 for window creation, theoretically works on a toaster
   - Some type abstractions for the sake of porting ease (eg. int64 on Android)
   - Tons of platform-abstraction, from files to networking (the latter is still WIP)
