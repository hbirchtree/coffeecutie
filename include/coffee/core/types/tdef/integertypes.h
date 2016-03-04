#pragma once

#include <stddef.h>
#include <stdint.h>

#include "../../coffee_mem_macros.h"

namespace Coffee{

using int8  = char;
using int16 = int16_t;
using int32 = int32_t;
using int64 = int64_t;

using uint8  = uint8_t;
using uint16 = uint16_t;
using uint32 = uint32_t;
using uint64 = uint64_t;

using scalar    = float;  /*!< Low-precision float value*/
using bigscalar = double; /*!< High-precision float value*/
using lscalar   = long double; /* Whoops... */

using byte_t    = uint8;  /*!< System byte*/
using ubyte_t   = byte_t; /*!< System byte*/
using sbyte_t   = int8;   /*!< System unsigned byte*/

using ushort = uint16;

using cstring    = const sbyte_t*; /*!< Typical C-string*/
using cstring_w  = sbyte_t*; /*!< Writable C-string*/
using cwstring   = const wchar_t*; /*!< Wide C-string*/
using cwstring_w = wchar_t*; /*!< Wide, writable C-string*/

using szptr = size_t; /*!< Usable as offset into data*/

constexpr int8 Int8_Max = (INT8_MAX-1);
constexpr int8 Int8_Min = (INT8_MIN+1);
constexpr uint8 UInt8_Max = (UINT8_MAX-1);

constexpr int16 Int16_Max = (INT16_MAX-1);
constexpr int16 Int16_Min = (INT16_MIN+1);
constexpr uint16 UInt16_Max = (UINT16_MAX-1);

constexpr int32 Int32_Max = (INT32_MAX-1);
constexpr int32 Int32_Min = (INT32_MIN+1);
constexpr uint32 UInt32_Max = (UINT32_MAX-1);

constexpr int64 Int64_Max = (INT64_MAX-1);
constexpr int64 Int64_Min = (INT64_MIN+1);
constexpr uint64 UInt64_Max = (UINT64_MAX-1);

using c_cptr = const void*;
using c_ptr  = void*;

PACKEDSTRUCT uint24
{
    uint32 d:24;
};

}