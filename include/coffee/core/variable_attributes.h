#pragma once

#include "plat/plat_primary_identify.h"
#include "plat/plat_compiler_identify.h"

#if (__cplusplus >= 201703L)
#define UNUSED_PARAM [[maybe_unused]]

#elif defined(COFFEE_GCC) || defined(COFFEE_CLANG) \
    && !defined(COFFEE_MAEMO)
#define UNUSED_PARAM __attribute__((unused))

#else
#define UNUSED_PARAM

#endif

#if (__cplusplus >= 201703L)
#define C_MAYBE_UNUSED [[maybe_unused]]

#elif (defined(COFFEE_GCC) || defined(COFFEE_CLANG)) \
    && !defined(COFFEE_MAEMO)
#define C_MAYBE_UNUSED __attribute__((unused))

#else
#define C_MAYBE_UNUSED
#endif

#if (__cplusplus >= 201403L)
#define C_DEPRECATED [[deprecated]]
#define C_DEPRECATED_S(reason) [[deprecated(reason)]]

#elif (defined(COFFEE_GCC) || defined(COFFEE_CLANG)) \
    && !defined(COFFEE_MAEMO)
#define C_DEPRECATED __attribute__((deprecated))
#define C_DEPRECATED_S(reason) C_DEPRECATED

#elif defined(COFFEE_MSVC)
#define C_DEPRECATED __declspec(deprecated)
#define C_DEPRECATED_S(reason) C_DEPRECATED

#else
#define C_DEPRECATED
#define C_DEPRECATED_S(reason)

#endif

#if !defined(COFFEE_MAEMO)
#define C_NORETURN [[noreturn]]

#else
#define C_NORETURN

#endif

#if (__cplusplus >= 201703L)
#define C_HAS_INCLUDE(header) __has_include(header)

#else
#define C_HAS_INCLUDE(header) 0

#endif

#define C_OPTIONAL
