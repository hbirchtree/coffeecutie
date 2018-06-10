# OpenGL support

 - Linux (tested on NVIDIA and Intel drivers)
       - OpenGL 3.3+
       - OpenGL ES 3.0+ (only simulated mode is supported for NVIDIA)
 - Windows (tested on NVIDIA and Intel drivers)
       - OpenGL 3.3+
       - OpenGL ES 3.0+
 - Windows UWP
       - OpenGL ES 2.0
 - Android
       - OpenGL ES 2.0, works with older SDK versions
       - OpenGL ES 3.0 (3D rendering APIs), needs SDK 18+
 - iOS
       - OpenGL ES 2.0
       - Technically should support OpenGL ES 3.0, but not tested
 - Mac OS X
       - OpenGL 3.3 (Next API level is at GL4.3)

## Simulating graphics modes
On supported platforms (Windows, Linux) you may also force a rendering mode.
Do note that this is a command-level simulation, and will not simulate hardware differences, such as differences in texture sampling or other bugs in mobile graphics drivers (of which there are many!).
If you want to use the simulation mode, specify the following:

        GLEAM_API=[level] ./program ...

Valid levels as of February 2018 are:

 - `GL33` = OpenGL 3.3
 - `GL43` = OpenGL 4.3
 - `GL45` = OpenGL 4.5
 - `ES2`  = OpenGL ES 2.0
 - `ES3`  = OpenGL ES 3.0
 - `ES32` = OpenGL ES 3.2
