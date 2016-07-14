/* ...And this is string processing in a pre-processor. Look away now. */
#define STR_UNREDIR(s) #s
#define STR_REDIR(s) STR_UNREDIR(s)

#define C_COMPILER_DEFINE_STRING(name,maj,min,lev) name " " STR_REDIR(maj) "." STR_REDIR(min) "." STR_REDIR(lev)

#undef C_SYSTEM_BITNESS
#define C_SYSTEM_BITNESS 0

/* Pre-processors are stupid. Very stupid. They would suffocate with a glass of water in their hand. */
#undef C_COMPILER_NAME
#undef C_COMPILER_VER_MAJ
#undef C_COMPILER_VER_MIN
#undef C_COMPILER_VER_REV

#define C_COMPILER_NAME "Unknown"
#define C_COMPILER_VER_MAJ 0
#define C_COMPILER_VER_MIN 0
#define C_COMPILER_VER_REV 0


/* GCC compiler identification */
#if defined(__GNUC__) && defined(__cplusplus)

#undef C_SYSTEM_BITNESS

#if __x86_64__ || __ppc64__
#define C_SYSTEM_BITNESS 64
#else
#define C_SYSTEM_BITNESS 32
#endif

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

/* Clang can identify as an apache attack helicopter at times. */
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

#if defined(_WIN32) || defined(_WIN64)
#undef C_SYSTEM_BITNESS
#define C_SYSTEM_BITNESS 32

#if defined(_WIN64)
#undef C_SYSTEM_BITNESS
#define C_SYSTEM_BITNESS 64
#endif
#endif

/* This wasn't so hard, really. */
#if defined(_MSC_VER) && defined(__cplusplus)

#define COFFEE_MSVCXX
#define C_COMPILER_STRING "MSVC++" _MSC_VER

#undef C_COMPILER_NAME
#undef C_COMPILER_VER_MAJ
#undef C_COMPILER_VER_MIN
#undef C_COMPILER_VER_REV

#define C_COMPILER_NAME "MSVC++"
#define C_COMPILER_VER_MAJ _MSC_FULL_VER
#define C_COMPILER_VER_MIN _MSC_BUILD
#define C_COMPILER_VER_REV 0

#endif
