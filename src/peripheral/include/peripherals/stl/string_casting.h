#pragma once

#include <peripherals/libc/string_ops.h>
#include <peripherals/stl/stlstring_ops.h>

#define CAST_TO_FUNCTION(type)                            \
    template<>                                            \
    inline type cast_string(CString const& str)           \
    {                                                     \
        return str::from_string<type>(str.c_str());       \
    }                                                     \
    inline type operator"" _##type(const char* v, size_t) \
    {                                                     \
        return cast_string<type>(v);                      \
    }

#define CAST_FROM_FUNCTION(type, converter) \
    template<>                              \
    inline CString cast_pod(type src)       \
    {                                       \
        return str::to_string<type>(src);   \
    }

#define CAST_BOTH_FUNCTION(type) \
    CAST_TO_FUNCTION(type)       \
    CAST_FROM_FUNCTION(type)

#define CAST_LOWER_FUNCTION(type)                                 \
    template<>                                                    \
    inline type cast_string(CString const& str)                   \
    {                                                             \
        return C_CAST<type>(str::from_string<type>(str.c_str())); \
    }                                                             \
    inline type operator"" _##type(const char* v, size_t)         \
    {                                                             \
        return cast_string<type>(v);                              \
    }

namespace stl_types {

template<typename T>
T cast_string(CString const& val)
{
    return str::from_string<T>(val.c_str());
}

template<typename T>
T cast_string(CWString const& val)
{
    return str::from_string<T>(val.c_str());
}

template<typename T>
CString cast_pod(T v)
{
    return str::convert::to_string<T>(v);
}

} // namespace stl_types

#undef CAST_LOWER_FUNCTION
#undef CAST_BOTH_FUNCTION
#undef CAST_TO_FUNCTION
#undef CAST_FROM_FUNCTION
