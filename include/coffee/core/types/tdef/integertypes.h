#pragma once

#include <stddef.h>
#include <stdint.h>

#include "../../coffee_mem_macros.h"

namespace Coffee{

using int8  = char;
using int16 = int16_t;
using int32 = int32_t;
using int64 = int64_t;

using i8 = int8;
using i16 = int16;
using i32 = int32;
using i64 = int64;

using uint8  = uint8_t;
using uint16 = uint16_t;
using uint32 = uint32_t;
using uint64 = uint64_t;

using u8 = uint8;
using u16 = uint16;
using u32 = uint32;
using u64 = uint64;

using hscalar   = int16;
using scalar    = float;  /*!< Low-precision float value*/
using bigscalar = double; /*!< High-precision float value*/
using lscalar   = long double; /* Whoops... How did this get here? */

using f16 = hscalar;
using f32 = scalar;
using f64 = bigscalar;

using byte_t    = uint8;  /*!< System byte*/
using ubyte_t   = byte_t; /*!< System byte*/
using sbyte_t   = int8;   /*!< System unsigned byte*/

/* For wstring */
using wbyte_t   = wchar_t;

using ushort = uint16;

using cstring    = const sbyte_t*; /*!< Typical C-string*/
using cstring_w  = sbyte_t*; /*!< Writable C-string*/
using cwstring   = const wchar_t*; /*!< Wide C-string*/
using cwstring_w = wchar_t*; /*!< Wide, writable C-string*/

using szptr = size_t; /*!< Usable as offset into data*/

using intptr = intptr_t;
using uintptr = uintptr_t;

#if defined(INT8_MAX) && defined(UINT8_MAX)
constexpr int8 Int8_Max = (INT8_MAX);
constexpr int8 Int8_Min = (INT8_MIN);
constexpr uint8 UInt8_Max = (UINT8_MAX);
#else
constexpr int8 Int8_Max = (127);
constexpr int8 Int8_Min = (127)-1;
constexpr uint8 UInt8_Max = (255);
#endif

#if defined(INT16_MAX) && defined(UINT16_MAX)
constexpr int16 Int16_Max = (INT16_MAX);
constexpr int16 Int16_Min = (INT16_MIN);
constexpr uint16 UInt16_Max = (UINT16_MAX);
#else
constexpr int16 Int16_Max = (32767);
constexpr int16 Int16_Min = (-32767)+1;
constexpr uint16 UInt16_Max = 65535;
#endif

#if defined(INT32_MAX) && defined(UINT32_MAX)
constexpr int32 Int32_Max = (INT32_MAX);
constexpr int32 Int32_Min = (INT32_MIN);
constexpr uint32 UInt32_Max = (UINT32_MAX);
#else
constexpr int32 Int32_Max = 2147483647;
constexpr int32 Int32_Min = (-2147483647)-1;
constexpr uint32 UInt32_Max = 4294967295;
#endif

#if defined(INT64_MAX) && defined(UINT64_MAX)
constexpr int64 Int64_Max = (INT64_MAX);
constexpr int64 Int64_Min = (INT64_MIN);
constexpr uint64 UInt64_Max = (UINT64_MAX);
#else
constexpr int64 Int64_Max = (9223372036854775807LL);
constexpr int64 Int64_Min = (-9223372036854775807LL)-1;
constexpr uint64 UInt64_Max = (18446744073709551615ULL);
#endif

using c_cptr = const void*;
using c_ptr  = void*;

#if !defined(COFFEE_RASPBERRYPI)
using ThrdCnt = uint64;
#else
using ThrdCnt = uint32;
#endif
using MemUnit = uint64;
using CoreCnt = uint32;

constexpr MemUnit Unit_kB = 1024;
constexpr MemUnit Unit_MB = 1024*1024;
constexpr MemUnit Unit_GB = 1024*1024*1024;
constexpr MemUnit Unit_TB = 1024ULL*1024ULL*1024ULL*1024ULL;

#define GEN_SIZE_LITERAL(unit) \
    inline MemUnit operator "" _ ## unit ## B (unsigned long long int v) \
    { \
        return v * Unit_ ## unit ## B;\
    }

GEN_SIZE_LITERAL(k)
GEN_SIZE_LITERAL(M)
GEN_SIZE_LITERAL(G)
GEN_SIZE_LITERAL(T)

PACKEDSTRUCT uint24
{
    uint32 d:24;
};

using EvTs = uint32;

}
