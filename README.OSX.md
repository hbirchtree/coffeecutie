# Port details
Reuses a lot of the POSIX-compliant code from the Linux version, with minor
modifications to some functions. (Such as file mapping,
where Linux defines its own rules)

# How to build and deploy for OS X?

The project requires C++11 and up compatibility which is not found with
Apple's older versions of Xcode.
I recommend Xcode 5.1+ at a *minimum*.
Some workarounds are in place in order to support older Xcode versions.
Thread-local storage uses the `__thread` attribute instead of `thread_local`,
which limits TLS to POD types.

I (the developer) do this:

0. Install Xcode CLI tools (eg. run `clang` in terminal once)
1. Install Ninja and CMake through homebrew

          brew install ninja cmake

2. Using `quick-build.sh`, compile the `osx` target.
3. Compile it! It should just work. If you experience problems, please report them.

# Any quirks?

When running as a Jenkins slave, CMake and Ninja should be
symlinked to `/usr/bin`. This ensures proper detection.

# Running on iOS

Most of the functionality for iOS works, including saving files to the
application's directory using bundle information and etc.
Rendering graphics is in need of a couple of fixes still.
