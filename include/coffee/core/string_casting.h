#pragma once

#include <coffee/core/plat/memory/stlstring_ops.h>
#include <coffee/core/plat/memory/string_ops.h>

#define CAST_TO_FUNCTION(type, converter) \
    template<> inline type cast_string(CString const& str) \
    { return Mem::Convert::converter(str.c_str()); } \
    inline type operator "" _ ## type(const char* v, unsigned long) \
    {return cast_string<type>(v);}

#define CAST_FROM_FUNCTION(type, converter) \
    template<> inline CString cast_pod(type src) {return Mem::Convert::converter(src); }

#define CAST_BOTH_FUNCTION(type, converter1, converter2) \
    CAST_TO_FUNCTION(type, converter1) \
    CAST_FROM_FUNCTION(type, converter2)

#define CAST_LOWER_FUNCTION(type, converter) \
    template<> inline type cast_string(CString const& str) \
    {return C_CAST<type>(Mem::Convert::converter(str.c_str())); } \
    inline type operator "" _ ## type(const char* v, unsigned long) \
    {return cast_string<type>(v);}

namespace Coffee{

template<typename T>
T cast_string(CString const&)
{
    return 0;
}

template<typename T>
CString cast_pod(T v)
{
    return Mem::Convert::intltostring(v);
}
    
template<size_t>
CString cast_pod(size_t v)
{
    return Mem::Convert::sizetostring(v);
}

CAST_BOTH_FUNCTION(uint64, strtoull, uintltostring)
CAST_BOTH_FUNCTION(int64, strtoll, intltostring)
CAST_BOTH_FUNCTION(uint32, strtouint, uinttostring)
CAST_BOTH_FUNCTION(int32, strtoint, inttostring)
CAST_BOTH_FUNCTION(scalar, strtofscalar, scalarftostring)
CAST_BOTH_FUNCTION(bigscalar, strtoscalar, scalartostring)
CAST_BOTH_FUNCTION(bool, strtobool, booltostring)

CAST_FROM_FUNCTION(int8, intctostring)
CAST_FROM_FUNCTION(uint8, uintctostring)
CAST_FROM_FUNCTION(int16, intstostring)
CAST_FROM_FUNCTION(uint16, uintstostring)

CAST_LOWER_FUNCTION(int8, strtoint)
CAST_LOWER_FUNCTION(uint8, strtouint)
CAST_LOWER_FUNCTION(int16, strtoint)
CAST_LOWER_FUNCTION(uint16, strtouint)

}

#undef CAST_LOWER_FUNCTION
#undef CAST_BOTH_FUNCTION
#undef CAST_TO_FUNCTION
#undef CAST_FROM_FUNCTION
