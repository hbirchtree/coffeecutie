# Port details
The Android port is mostly C++, with the exception of some code hooking into Java to allow tighter integration. For example, Java sets the native window to immersive full-screen, which is a bit tricky from Java.

In general, all input handling, EGL handling and lifecycle events are done in pure C++, application-side at least.

One restriction for now is that only EGL support is existent. Vulkan support shouldn't be too hard, but it would require further separation in the Android backend.

Android builds support bare-metal builds in `quick-build.sh`.
