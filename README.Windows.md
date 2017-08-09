# How to build and deploy for Windows?

You *will* need Visual Studio 2015 in order for compilation to work. (We need full C++11 support, not what VS2013 provides) Compiling 32-bit will not be supported officially; if that breaks, nobody cares.

No successful attempts have been made with Cygwin, MinGW-W64 and MSYS2 so far. (Please report back if it does work!)

For setting up an environment, refer to `BUILDING.md` on Windows Makefiles,
as it is usable in Visual Studio.

**NB**: Note that Windows is not fully supported or tested at all times.

# Platform implementation
Uses Win32 where Microsoft won't let us use POSIX. Reuses POSIX functions where possible, although this is not always the case.
