#pragma once

#include "plat/plat_compiler_identify.h"

#if (__cplusplus >= 201703L)
#define UNUSED_PARAM [[maybe_unused]]
#else
#if defined(COFFEE_GCC) || defined(COFFEE_CLANG)
#define UNUSED_PARAM __attribute__((unused))
#else
#define UNUSED_PARAM
#endif
#endif

#if (__cplusplus >= 201703L)
#define C_MAYBE_UNUSED [[maybe_unused]]
#else
#define C_MAYBE_UNUSED
#endif

#if (__cplusplus >= 201403L)
#define C_DEPRECATED [[deprecated]]
#define C_DEPRECATED_S(reason) [[deprecated(reason)]]
#else
#if defined(COFFEE_GCC) || defined(COFFEE_CLANG)
#define C_DEPRECATED __attribute__((deprecated))
#define C_DEPRECATED_S(reason) C_DEPRECATED
#elif defined(COFFEE_MSVC)
#define C_DEPRECATED __declspec(deprecated)
#define C_DEPRECATED_S(reason) C_DEPRECATED
#else
#pragma message("No warnings for deprecated functions!")
#define C_DEPRECATED
#endif
#endif

#define C_NORETURN [[noreturn]]

#if (__cplusplus >= 201703L)
#define C_HAS_INCLUDE(header) __has_include(header)
#else
#define C_HAS_INCLUDE(header) 0
#endif
