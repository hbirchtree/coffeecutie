#ifdef __linux__

#define COFFEE_LINUX

#endif

#ifdef _WIN32

#define COFFEE_WINDOWS

#endif

#if defined(__APPLE__) && defined(__MACH__)

#define COFFEE_APPLE

#endif

#ifdef __unix__

#define COFFEE_UNIXPLAT

#endif
