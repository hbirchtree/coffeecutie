#pragma once

#include <limits>
#include <stddef.h>
#include <stdint.h>

#include <peripherals/identify/compiler/function_inlining.h>
#include <peripherals/identify/compiler/struct_packing.h>
#include <peripherals/identify/compiler/variable_attributes.h>

namespace libc_types {

#if defined(COFFEE_PLAIN_INT_TYPES)
using int32_t  = int;
using uint32_t = unsigned int;
#endif

using i8  = int8_t;
using i16 = int16_t;
using i32 = int32_t;
using i64 = int64_t;

using u8  = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using u64 = uint64_t;

using hscalar   = i16;
using scalar    = float;       /*!< Low-precision float value*/
using bigscalar = double;      /*!< High-precision float value*/
using lscalar   = long double; /* Whoops... How did this get here? */

using f16 = hscalar;
using f32 = scalar;
using f64 = bigscalar;
using f80 = lscalar;

using byte_t  = u8;     /*!< System byte*/
using ubyte_t = byte_t; /*!< System byte*/
using sbyte_t = i8;     /*!< System unsigned byte*/

/* For wstring */
using wbyte_t = wchar_t;

using ushort = u16;

using cstring    = const char*;    /*!< Typical C-string*/
using cstring_w  = char*;          /*!< Writable C-string*/
using cwstring   = const wchar_t*; /*!< Wide C-string*/
using cwstring_w = wchar_t*;       /*!< Wide, writable C-string*/
using u8string   = const uint8_t*; /*!< UTF-8 string */
using u8string_w = uint8_t*;       /*!< Writable UTF-8 string */

using szptr  = size_t;   /*!< Usable as offset into data, or size of data */
using ptroff = intptr_t; /*!< Signed offset into data */

using intptr  = intptr_t;
using uintptr = uintptr_t;

using c_cptr = const void*;
using c_ptr  = void*;

#if !defined(COFFEE_RASPBERRYPI)
using ThrdCnt = u64;
#else
using ThrdCnt               = uint32;
#endif
using MemUnit = u64;

constexpr u16 Unit_kB = 1024;
constexpr u32 Unit_MB = 1024 * 1024;
constexpr u32 Unit_GB = 1024 * 1024 * 1024;
constexpr u64 Unit_TB = 1024ULL * 1024ULL * 1024ULL * 1024ULL;

#define GEN_SIZE_LITERAL(unit)                                               \
    constexpr inline u64 operator"" _##unit##B(unsigned long long int v) \
    {                                                                        \
        return v * Unit_##unit##B;                                           \
    }

namespace size_literals {

GEN_SIZE_LITERAL(k)
GEN_SIZE_LITERAL(M)
GEN_SIZE_LITERAL(G)
GEN_SIZE_LITERAL(T)

} // namespace size_literals

#define GEN_TIME_LITERAL(unit, div)                                            \
    constexpr inline bigscalar operator"" _##unit##s(unsigned long long int v) \
    {                                                                          \
        return bigscalar(v) / div;                                             \
    }

GEN_TIME_LITERAL(m, 1000)
GEN_TIME_LITERAL(u, 1000000)

PACKEDSTRUCT(uint24 { u32 d : 24; });

using u24 = uint24;

FORCEDINLINE scalar convert_i16_f(i16 v)
{
    return static_cast<scalar>(v) / std::numeric_limits<i16>::max();
}

template<typename T>
FORCEDINLINE T convert_f32(f32 v)
{
    return static_cast<T>(v * std::numeric_limits<T>::max());
}
} // namespace libc_types
