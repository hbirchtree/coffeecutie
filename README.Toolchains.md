# Platform compiler details

You should use this list when you consider using C++ features introduced
in C++14 and later.
Some features are special cases and quite ugly. Beware.

## Compiler versions and workarounds

| Platform         | Compiler name       | Compiler version |Workarounds       |
|:-----------------|--------------------:|:-----------------|:-----------------|
|Android           | Clang (GCC libs)    | NDK 15c          |7                 |
|Apple iOS         | Apple/Clang         | Xcode 10/11      |                  |
|Apple OS X        | Apple/Clang         | Xcode 10/11      |7                 |
|Emscripten        | Clang/emcc          | Latest           |2.1, 7, 8         |
|Maemo 5           | GCC                 | 6.0.1            |3, 5, 6, 7, 8     |
|Raspberry Pi      | GCC                 | 9.2.1            |                  |
|SteamOS           | GCC                 | 4.8.1            |10                |
|Fedora            | GCC                 | 9.2.1            |                  |
|Ubuntu            | GCC                 | 7.4.0            |                  |
|Ubuntu (alt.)     | Clang               | 8.0.0            |                  |
|Windows (Win32)   | MSVC++              | MSVC 19          |4                 |
|Windows (UWP)     | MSVC++              | MSVC 19          |4                 |
|Nintendo Gekko    | GCC                 | 8.3.0            |9                 |
|                  |                     |                  |                  |

\* Only necessary with Xcode versions before 8

## Platform workarounds

| #    | Notes                                                                 |
|:-----|:----------------------------------------------------------------------|
| 2.1  | Removes uses of `thread_local` from code, no fallback                 |
| 3    | `std::future` is not implemented, replaced by single-thread solution. |
| 4    | OpenSSL support disabled, plain sockets only                          |
| 5    | Replace `RUSAGE_THREAD` with `RUSAGE_SELF`                            |
| 6    | Disables `MAP_HUGETLB`                                                |
| 7    | No `system()` function defined                                        |
| 7.1  | No `execvpe()` function defined, external programs will fail          |
| 8    | No way to set thread names, `pthread_setname_np()` non-existent       |
| 9    | Custom, platform-bound thread and atomics layer                       |
| 10   | std::regex is not properly implemented                                |

## CMake versions (for Dockerized and CI builds)

| Platform         | CMake version   | Notes                                   |
|:-----------------|----------------:|:----------------------------------------|
|Android           |3.16.2           |Runs on Ubuntu Bionic                    | 
|Apple iOS/OSX     |Recent version   |Pulled from Brew                         | 
|Emscripten        |3.16.2           |Runs on Ubuntu Xenial                    | 
|Fedora            |3.16.2           |Runs on Fedora 28                        | 
|Maemo 5           |3.1.0            |Latest compatible version for Maemo      | 
|Raspberry Pi      |3.16.2           |Runs on Ubuntu Bionic                    | 
|SteamOS           |3.2.2            |Runs on Ubuntu Trusty                    | 
|Ubuntu            |3.16.2           |Runs on Ubuntu Bionic                    | 
|Windows (all)     |Recent version   |Pulled from Chocolatey                   | 
|                  |                 |                                         |

