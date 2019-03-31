# Port details
The Android port is mostly C++, with the exception of some code hooking into Java to allow tighter integration. For example, Java sets the native window to immersive full-screen, which is a bit tricky from C++.

The Java code used for this application is solely contained in `toolchain/desktop/android`.

In general, all input handling, EGL handling and lifecycle events are done in pure C++, application-side at least.

One restriction for now is that only EGL support is existent. Vulkan support shouldn't be too hard, but it would require further separation in the Android backend.

Android builds support bare-metal builds in `quick-build.sh`.

# Debugging the app

In general, you should be able to open a *generated* Gradle project from `${BUILD_DIR}/deploy/android-apk/${target_name}`. This may sometimes not work. Below are some issues and solutions.

## The app does not trigger a breakpoint in debug mode!

Sometimes, Android Studio does not start LLDB *at all*. To avoid this, go into `Run > Edit Configurations > app > Debugger`, where you will find the `Debug type` combo box. The best choice is to use `Native` when debugging an application with mostly C++ code.
