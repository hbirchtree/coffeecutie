# Platform compiler details

You should use this list when you consider using C++ features introduced in C++14 and later.
Some features are special cases and quite ugly. Beware.

## Compiler versions and workarounds

| Platform         | Compiler name       | Compiler version |Workarounds       |
|:-----------------|--------------------:|:-----------------|:-----------------|
|Apple iOS         | Clang               | 3.4.0+           |2                 |
|Apple OS X        | Clang               | 3.4.0+           |1, 7              |
|Emscripten        | emcc                | 1.37.0+          |2.1, 7, 7.1, 8    |
|Maemo 5           | GCC                 | 6.0.1            |3, 5, 6, 7, 7.1, 8|
|Native Client     | Clang               | 3.7.0+           |2.1, 7, 7.1, 8    |
|Raspberry Pi      | GCC                 | 4.9.3            |                  |
|SteamOS           | GCC                 | 4.8.1            |                  |
|Ubuntu            | GCC                 | 5.4.0+           |                  |
|Ubuntu (alt.)     | Clang               | 3.8.0+           |                  |
|Windows (Win32)   | MSVC++              | 14.0             |4                 |
|Windows (UWP)     | MSVC++              | 14.0             |4                 |
|                  |                     |                  |                  |

## Platform workarounds

| #    | Notes                                                                 |
|:-----|:----------------------------------------------------------------------|
| 1    | Replaces `thread_local` with `__thread`, only supports POD types      |
| 2    | Removes usage of `thread_local`, `__thread` not supported             |
| 2.1  | Removes uses of `thread_local` from code, no fallback                 |
| 3    | `std::future` is not implemented, replaced by single-thread solution. |
| 4    | OpenSSL support disabled, plain sockets only                          |
| 5    | Replace `RUSAGE_THREAD` with `RUSAGE_SELF`                            |
| 6    | Disables `MAP_HUGETLB`                                                |
| 7    | No `execvpe()` function defined, external programs will fail            |
| 7.1  | No `system()` function defined                                          |
| 8    | No way to set thread names, `pthread_setname_np()` non-existent         |
