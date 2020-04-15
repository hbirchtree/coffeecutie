#pragma once

#include <peripherals/identify/compiler.h>
#include <peripherals/identify/system.h>

#if defined(__has_cpp_attribute)
#define VERIFY_CPP_FEATURE(version, attrib) \
    (__cplusplus >= version) || (__has_cpp_attribute(attrib))
#else
#define VERIFY_CPP_FEATURE(version, attrib) (__cplusplus >= version)
#endif

/*
 * [[maybe_unused]], for parameters
 *
 */
#if VERIFY_CPP_FEATURE(201703L, maybe_unused)
#define UNUSED_PARAM(type, name) [[maybe_unused]] type name

#elif defined(COFFEE_GCC) || defined(COFFEE_CLANG)
#define UNUSED_PARAM(type, name) __attribute__((unused)) type name

#else
#define UNUSED_PARAM(type, name) type name

#endif

/*
 * [[maybe_unused]], for variables
 *
 */
#if VERIFY_CPP_FEATURE(201703L, maybe_unused)
#define C_UNUSED(var_decl) [[maybe_unused]] var_decl

#elif defined(COFFEE_GCC) || defined(COFFEE_CLANG)
#define C_UNUSED(var_decl) __attribute__((unused)) var_decl

#else
#define C_UNUSED(var_decl) var_decl
#endif

/*
 * [[deprecated]]
 *
 */
#if VERIFY_CPP_FEATURE(201403L, deprecated)
#define C_DEPRECATED [[deprecated]]
#define C_DEPRECATED_S(reason) [[deprecated(reason)]]

#elif(defined(COFFEE_GCC) && __GNUC_MAJOR > 4) || defined(COFFEE_CLANG)
#define C_DEPRECATED __attribute__((deprecated))
#define C_DEPRECATED_S(reason) __attribute__((deprecated(reason, "")))

#elif defined(COFFEE_MSVC)
#define C_DEPRECATED __declspec(deprecated)
#define C_DEPRECATED_S(reason) __declspec(deprecated(reason))

#else
#define C_DEPRECATED
#define C_DEPRECATED_S(reason)

#endif

/*
 * [[fallthrough]]
 *
 */
#if VERIFY_CPP_FEATURE(201703L, fallthrough)
#define C_FALLTHROUGH [[fallthrough]]

#else
#define C_FALLTHROUGH

#endif

/*
 * [[noreturn]]
 *
 */
#define C_NORETURN [[noreturn]]

/*
 * [[nodiscard]]
 *
 */
#if VERIFY_CPP_FEATURE(201703L, nodiscard)
#define NO_DISCARD [[nodiscard]]

#elif defined(COFFEE_GCC)
#define NO_DISCARD [[gnu::warn_unused_result]]

#elif defined(COFFEE_CLANG)
#define NO_DISCARD [[clang::warn_unused_result]]

#elif defined(COFFEE_MSVC)
#define NO_DISCARD _Must_inspect_result_

#else
#define NO_DISCARD

#endif

/*
 * has_include
 *
 */
#if __cplusplus >= 201703L
#define C_HAS_INCLUDE(header) __has_include(header)
#else
#define C_HAS_INCLUDE(header) (0)

#endif

#define C_OPTIONAL

template<typename T>
struct C_DEPRECATED_TYPE
{
    /*C_DEPRECATED*/ typedef T type;
};

#undef VERIFY_CPP_FEATURE

#if defined(COFFEE_CLANG) || defined(COFFEE_GCC)
#define COFFEE_DISABLE_ASAN __attribute__((no_sanitize("address")))
#else
#define COFFEE_DISABLE_ASAN
#endif
