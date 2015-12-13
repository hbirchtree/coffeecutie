#ifdef __linux__

#define COFFEE_LINUX

//For the sake of upcoming window systems
#define COFFEE_X11
//#define COFFEE_WAYLAND
//#define COFFEE_MIR

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
