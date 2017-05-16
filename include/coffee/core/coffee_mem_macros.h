#ifndef COFFEE_CORE_MEM_MACROS_H
#define COFFEE_CORE_MEM_MACROS_H

#include <type_traits>
#include <limits>

#include "plat/plat_compiler_identify.h"
#include "plat/plat_quirks_toggling.h"

#define C_DEFAULT_CONSTRUCTOR(ctype) ctype() = default
#define C_DELETE_COPY_CONSTRUCTOR(ctype) ctype(ctype const&) = delete
#define C_MOVE_CONSTRUCTOR(ctype) ctype(ctype&& x) = default; ctype& operator=(ctype&& a) = default

#if defined(COFFEE_MACRO_CASTS)
#define C_CAST(type, var) static_cast<type>(var)
#define C_DCAST(type, var) dynamic_cast<type>(var)
#else

template <typename D, typename T>
static inline typename std::enable_if<std::is_signed<D>::value, bool>::type C_ICAST(T from)
{
    using valid_T1 = typename std::enable_if<std::is_unsigned<T>::value, bool>::type;

    valid_T1 v1 = 0;

    return ((from > std::numeric_limits<D>::max())
            ? 0
            : from) + v1;
}
template <typename D, typename T>
static inline typename std::enable_if<std::is_unsigned<D>::value, bool>::type C_ICAST(T from)
{
    using valid_T1 = typename std::enable_if<std::is_signed<T>::value, bool>::type;

    valid_T1 v1 = 0;

    return ((from < 0)
            ? 0
            : from) + v1;
}

template<typename D, typename T>
static inline D C_FCAST(T from)
{
    using T_not_const = typename std::remove_cv<T>::type;
    return reinterpret_cast<D>(const_cast<T_not_const>(from));
}

template<typename D, typename T>
static inline D C_FCAST(T* from)
{

    using T_not_const =
    typename std::add_pointer<
    typename std::remove_cv<
    typename std::remove_pointer<T>::type>::type>::type;

    return reinterpret_cast<D>(const_cast<T_not_const>(from));
}

template<typename D, typename T>
static inline D C_CAST(T from)
{
    return static_cast<D>(from);
}
template<typename D, typename T>
static inline D* C_DCAST(T* from)
{
    return dynamic_cast<D*>(from);
}
template<typename D, typename T>
static inline D C_CCAST(T from)
{
    return const_cast<D>(from);
}

#define C_CAST(type, var) C_CAST<type>(var)
#define C_DCAST(type, var) C_DCAST<type>(var)
#endif

/* Library export */
#if defined(COFFEE_GCC) || defined(COFFEE_CLANG)

#if !defined(COFFEE_NO_EXPORT)
#define COFFAPI extern "C" __attribute__((visibility("default")))
#define COFFAPI_PP extern __attribute__((visibility("default")))
#else
#define COFFAPI extern "C"
#define COFFAPI_PP extern
#endif

#elif defined(COFFEE_MSVCXX)

#if !defined(COFFEE_NO_EXPORT)
#define COFFAPI extern "C" __declspec(dllexport)
#else
#define COFFAPI extern "C"
#endif

#else

#define COFFAPI extern "C"

#endif

/* Force packing, compiler flags */
#if defined(COFFEE_GCC) || defined(COFFEE_CLANG)
#define C_FORCE_PACKING __attribute__((packed))
#elif defined(COFFEE_MSVCXX)
#include "plat/plat_windows.h"
//TODO: Fix struct packing on Windows, this is broken
#define C_FORCE_PACKING //UNALIGNED
#else
#error "You done diddled it again; there is no packing defined!"
#endif

/* Force inlining, compiler flags */
#if (defined(COFFEE_GCC) || defined(COFFEE_CLANG)) && !defined(COFFEE_ANDROID)
#define C_FORCE_INLINE __attribute__((always_inline))
#elif defined(COFFEE_MSVCXX)
#include "plat/plat_windows.h"
#define C_FORCE_INLINE __forceinline
#else
#define C_FORCE_INLINE
//#pragma message("Warning: Forced inlining not defined!")
#endif

#if defined(COFFEE_GCC) || defined(COFFEE_CLANG)
#define C_FORCE_NOTINLINE __attribute__((noinline))
#else
#define C_FORCE_NOTINLINE
//#pragma message("Warning: Some functions may be inlined unintentionally because of this undefined macro.")
#endif

/* To disable inline */
#define DENYINLINE C_FORCE_NOTINLINE

/* Force inlining macros */
#if !defined(COFFEE_LIMIT_INLINE)
#define STATICINLINE inline C_FORCE_INLINE static
#define FORCEDINLINE inline C_FORCE_INLINE
#else
#define STATICINLINE inline DENYINLINE static
#define FORCEDINLINE inline DENYINLINE
#endif

/* Forced packing, required for some types to work (uint24 etc.) */
#define PACKEDSTRUCT struct C_FORCE_PACKING

#endif

/* Apparently, a lot of compilers define these shits */
#undef major
#undef minor

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
#else
#if defined(COFFEE_GCC) || defined(COFFEE_CLANG)
#define C_DEPRECATED __attribute__((deprecated))
#elif defined(COFFEE_MSVC)
#define C_DEPRECATED __declspec(deprecated)
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
