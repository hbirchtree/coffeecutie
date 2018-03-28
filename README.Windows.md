# How to build and deploy for Windows?

You *will* need Visual Studio 2015 in order for compilation to work. (We need full C++11 support, not what VS2013 provides) Compiling 32-bit will not be supported officially; if that breaks, nobody cares.

It is also possible to build for MinGW-W64, but this is not a focus since it brings with it a lot of extra DLLs. It is available through the `quick-build.sh` target called `mingw.w64`.

For setting up an environment, refer to `BUILDING.md` on Windows Makefiles,
as it is usable in Visual Studio.

**NB**: Note that Windows is not fully supported or tested at all times.

# Platform implementation
Uses Win32 where Microsoft won't let us use POSIX. Reuses POSIX functions where possible, although this is not always the case.
