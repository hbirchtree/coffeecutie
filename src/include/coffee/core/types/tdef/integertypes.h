#pragma once

#include <stddef.h>
#include <stdint.h>

#include "../../coffee_mem_macros.h"

namespace Coffee {
namespace Primitives {

#if defined(COFFEE_PLAIN_INT_TYPES)
using int32_t  = int;
using uint32_t = unsigned int;
#endif

using i8     = C_DEPRECATED_TYPE<int8_t>::type;
using int16  = C_DEPRECATED_TYPE<int16_t>::type;
using int32  = C_DEPRECATED_TYPE<int32_t>::type;
using int64  = C_DEPRECATED_TYPE<int64_t>::type;
using uint8  = C_DEPRECATED_TYPE<uint8_t>::type;
using uint16 = C_DEPRECATED_TYPE<uint16_t>::type;
using uint32 = C_DEPRECATED_TYPE<uint32_t>::type;
using uint64 = C_DEPRECATED_TYPE<uint64_t>::type;

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

using szptr  = size_t; /*!< Usable as offset into data*/
using ptroff = intptr_t;

using intptr  = intptr_t;
using uintptr = uintptr_t;

#if defined(INT8_MAX) && defined(UINT8_MAX)
constexpr i8 Int8_Max  = (INT8_MAX);
constexpr i8 Int8_Min  = (INT8_MIN);
constexpr u8 UInt8_Max = (UINT8_MAX);
#else
constexpr int8   Int8_Max   = (127);
constexpr int8   Int8_Min   = (127) - 1;
constexpr uint8  UInt8_Max  = (255);
#endif

#if defined(INT16_MAX) && defined(UINT16_MAX)
constexpr i16 Int16_Max  = (INT16_MAX);
constexpr i16 Int16_Min  = (INT16_MIN);
constexpr u16 UInt16_Max = (UINT16_MAX);
#else
constexpr int16  Int16_Max  = (32767);
constexpr int16  Int16_Min  = (-32767) + 1;
constexpr uint16 UInt16_Max = 65535;
#endif

#if defined(INT32_MAX) && defined(UINT32_MAX)
constexpr i32 Int32_Max  = (INT32_MAX);
constexpr i32 Int32_Min  = (INT32_MIN);
constexpr u32 UInt32_Max = (UINT32_MAX);
#else
constexpr int32  Int32_Max  = 2147483647;
constexpr int32  Int32_Min  = (-2147483647) - 1;
constexpr uint32 UInt32_Max = 4294967295;
#endif

#if defined(INT64_MAX) && defined(UINT64_MAX)
constexpr i64 Int64_Max  = (INT64_MAX);
constexpr i64 Int64_Min  = (INT64_MIN);
constexpr u64 UInt64_Max = (UINT64_MAX);
#else
constexpr int64  Int64_Max  = (9223372036854775807LL);
constexpr int64  Int64_Min  = (-9223372036854775807LL) - 1;
constexpr uint64 UInt64_Max = (18446744073709551615ULL);
#endif

using c_cptr = const void*;
using c_ptr  = void*;

#if !defined(COFFEE_RASPBERRYPI)
using ThrdCnt = u64;
#else
using ThrdCnt               = uint32;
#endif
using MemUnit = u64;
using CoreCnt = u32;

constexpr MemUnit Unit_kB = 1024;
constexpr MemUnit Unit_MB = 1024 * 1024;
constexpr MemUnit Unit_GB = 1024 * 1024 * 1024;
constexpr MemUnit Unit_TB = 1024ULL * 1024ULL * 1024ULL * 1024ULL;

#define GEN_SIZE_LITERAL(unit)                                     \
    inline MemUnit operator"" _##unit##B(unsigned long long int v) \
    {                                                              \
        return v * Unit_##unit##B;                                 \
    }

GEN_SIZE_LITERAL(k)
GEN_SIZE_LITERAL(M)
GEN_SIZE_LITERAL(G)
GEN_SIZE_LITERAL(T)

#define GEN_TIME_LITERAL(unit, div)                                  \
    inline bigscalar operator"" _##unit##s(unsigned long long int v) \
    {                                                                \
        return bigscalar(v) / div;                                   \
    }

GEN_TIME_LITERAL(m, 1000)
GEN_TIME_LITERAL(u, 1000000)

PACKEDSTRUCT uint24
{
    u32 d : 24;
};

using u24 = uint24;

FORCEDINLINE scalar convert_i16_f(i16 v)
{
    return C_CAST<scalar>(v) / Int16_Max;
}
}

using namespace Primitives;

} // namespace Coffee
