#pragma once

#include <coffee/core/plat/memory/stlstring_ops.h>
#include <coffee/core/plat/memory/string_ops.h>

#define CAST_TO_FUNCTION(type, converter) \
    template<> type cast_string(CString const& str) { return Mem::Convert::converter(str.c_str()); }

#define CAST_FROM_FUNCTION(type, converter) \
    template<> CString cast_pod(type src) {return Mem::Convert::converter(src); }

#define CAST_BOTH_FUNCTION(type, converter1, converter2) \
    CAST_TO_FUNCTION(type, converter1) \
    CAST_FROM_FUNCTION(type, converter2)

namespace Coffee{

template<typename T>
T cast_string(CString const&)
{
    return 0;
}

template<typename T>
CString cast_pod(T)
{
    return {};
}

CAST_BOTH_FUNCTION(uint64, strtoull, uintltostring)
CAST_BOTH_FUNCTION(int64, strtoll, intltostring)
CAST_BOTH_FUNCTION(uint32, strtouint, uinttostring)
CAST_BOTH_FUNCTION(int32, strtoint, inttostring)
CAST_BOTH_FUNCTION(scalar, strtofscalar, scalarftostring)
CAST_BOTH_FUNCTION(bigscalar, strtoscalar, scalartostring)

CAST_FROM_FUNCTION(bool, booltostring)
CAST_FROM_FUNCTION(int8, intctostring)
CAST_FROM_FUNCTION(uint8, uintctostring)
CAST_FROM_FUNCTION(int16, intstostring)
CAST_FROM_FUNCTION(uint16, uintstostring)


}

#undef CAST_TO_FUNCTION
