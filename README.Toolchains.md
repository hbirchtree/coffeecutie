# Platform compiler details

You should use this list when you consider using C++ features introduced in C++14 and later.
Some features are special cases and quite ugly. Beware.

## Compiler versions and workarounds

| Platform         | Compiler name       | Compiler version |Notes             |
|:-----------------|--------------------:|:-----------------|:-----------------|
|Apple iOS         | Clang               | 3.4.0+           |Workaround #2     |
|Apple OS X        | Clang               | 3.4.0+           |Workaround #1     |
|Emscripten        | emcc                | 1.37.0+          |                  |
|Native Client     | Clang               | 3.7.0+           |                  |
|Raspberry Pi      | GCC                 | 4.9.3            |                  |
|SteamOS           | GCC                 | 4.8.1            |                  |
|Ubuntu            | GCC                 | 5.4.0+           |                  |
|Ubuntu (alt.)     | Clang               | 3.8.0+           |                  |
|Windows (Win32)   | MSVC++              | 14.0             |                  |
|Windows (UWP)     | MSVC++              | 14.0             |                  |
|                  |                     |                  |                  |
|                  |                     |                  |                  |
|                  |                     |                  |                  |
|                  |                     |                  |                  |
|                  |                     |                  |                  |
|                  |                     |                  |                  |
|                  |                     |                  |                  |

## Platform workarounds

| #    | Notes                                                        |
|:-----|:-------------------------------------------------------------|
| 1    | Replaces thread_local with __thread, only supports POD types |
| 2    | Removes usage of thread_local, __thread not supported        |
