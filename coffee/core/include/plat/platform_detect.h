/* Linux macro, also defines window system target */
#if defined(__linux__)
#undef C_SYSTEM_STRING
#define C_SYSTEM_STRING "Linux"
#define COFFEE_LINUX

#if !defined(__ANDROID__)
#define COFFEE_X11
//#define COFFEE_WAYLAND
//#define COFFEE_MIR
#endif
#endif

#if defined(__ANDROID__)
#undef C_SYSTEM_STRING
#define C_SYSTEM_STRING "Android API level " __ANDROID_API__
#define COFFEE_ANDROID
#endif

/* Windows macro, only one for now */
#if defined(_WIN32)
#define C_SYSTEM_STRING "Windows"
#define COFFEE_WINDOWS
#endif

/* Mac OS X macro, not iOS */
#if defined(__APPLE__) && defined(__MACH__)
#define C_SYSTEM_STRING "Mac OS X"
#define COFFEE_APPLE
#endif

/* Generic UNIX, mostly for POSIX libraries */
#if defined(__unix__)
#define COFFEE_UNIXPLAT
#endif

#define STR_UNREDIR(s) #s
#define STR_REDIR(s) STR_UNREDIR(s)

#define C_COMPILER_DEFINE_STRING(name,maj,min,lev) name " " STR_REDIR(maj) "." STR_REDIR(min) "." STR_REDIR(lev)

#undef C_COMPILER_NAME
#define C_COMPILER_NAME "Unknown"

/* GCC compiler identification */
#if defined(__GNUC__) && defined(__cplusplus)
#define COFFEE_GCC

#undef C_COMPILER_NAME
#undef C_COMPILER_VER_MAJ
#undef C_COMPILER_VER_MIN
#undef C_COMPILER_VER_REV

#define C_COMPILER_NAME "GCC/G++"
#define C_COMPILER_VER_MAJ __GNUC__
#define C_COMPILER_VER_MIN __GNUC_MINOR__
#define C_COMPILER_VER_REV __GNUC_PATCHLEVEL__
#endif

#if defined(__clang__) && defined(__cplusplus)
#undef COFFEE_GCC
#define COFFEE_CLANG

#undef C_COMPILER_NAME
#undef C_COMPILER_VER_MAJ
#undef C_COMPILER_VER_MIN
#undef C_COMPILER_VER_REV

#define C_COMPILER_NAME "Clang"
#define C_COMPILER_VER_MAJ __clang_major__
#define C_COMPILER_VER_MIN __clang_minor__
#define C_COMPILER_VER_REV __clang_patchlevel__
#endif

#if defined(__MSC_VER) && defined(__cplusplus)
#define COFFEE_MSVCXX
#define C_COMPILER_STRING "MSVC++" _MSC_VER

#undef C_COMPILER_NAME
#undef C_COMPILER_VER_MAJ
#undef C_COMPILER_VER_MIN
#undef C_COMPILER_VER_REV

#define C_COMPILER_NAME "MSVC++"
#define C_COMPILER_VER_MAJ _MSC_FULL_VER
#define C_COMPILER_VER_MIN _MSC_BUILD
#define C_COMPILER_VER_REV x

#endif

//AMD64 string
#if defined(__amd64__)
#define COFFEE_ARCH "AMD64"

#elif defined(__i386__) || defined(_X86_)
#if defined(__i686__)
#define COFFEE_ARCH "i686"
#elif defined(__i586__)
#define COFFEE_ARCH "i586"
#elif defined(__i486__)
#define COFFEE_ARCH "i486"
#else
#define COFFEE_ARCH "i386"
#endif

#elif defined(__aarch64__)
#define COFFEE_ARCH "ARMv8-64"

#elif defined(__arm__)
#if defined(__ARM_ARCH_5__)
#define COFFEE_ARCH "ARMv5"
#elif defined(__ARM_ARCH_7A__)
#define COFFEE_ARCH "ARMv7a"
#elif defined(__thumb__)
#define COFFEE_ARCH "ARM Thumb mode"
#else
#define COFFEE_ARCH "ARM"
#endif

#else
#define COFFEE_ARCH "Unknown"
#endif

#if !defined(COFFEE_ANDROID)
#define COFFEE_USE_STL_TO_STRING
#endif

#if defined(COFFEE_UNIXPLAT) && !defined(COFFEE_ANDROID)
#define COFFEE_USE_UNWIND
#define COFFEE_USE_TERMINAL_CTL
#endif

#if defined(COFFEE_LINUX) && !defined(COFFEE_ANDROID)
#define COFFEE_USE_IOCTL_TERM_SIZE
#endif