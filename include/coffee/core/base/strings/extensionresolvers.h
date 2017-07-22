#ifndef COFFEE_CORE_BASE_DEBUG_EXT_RESOLVERS_H
#define COFFEE_CORE_BASE_DEBUG_EXT_RESOLVERS_H

#include "../../plat/memory/stlstring_ops.h"
#include "../../coffee_mem_macros.h"
#include "../../plat/plat_quirks_toggling.h"
#include "../../plat/memory/stlstring_ops.h"
#include "../../string_casting.h"

namespace Coffee{
namespace Strings{

using namespace Mem;

template<typename CharT,
         typename std::enable_if<std::is_integral<CharT>::value,
                                 bool>::type* = nullptr,
         typename std::enable_if<std::is_signed<CharT>::value,
                                 bool>::type* = nullptr>
inline CString to_string(const CharT* const& v)
{
    CString out;
    if(v)
        out = v;
    else
        out = "0x0";
    return out;
}

inline cstring to_string(bool const& v)
{
    return (v) ? "true" : "false";
}

template<typename CharT>
inline CString to_string(typename std::basic_string<CharT> const& v)
{
    return v;
}

template<typename T,
         typename std::enable_if<std::is_pointer<T>::value,
                                 bool>::type* = nullptr>
inline CString to_string(T const& v)
{
    return StrUtil::pointerify(v);
}

template<typename T,
         typename std::enable_if<std::is_integral<T>::value,
                                 bool>::type* = nullptr>
inline CString to_string(const T& v)
{
    return cast_pod<T>(v);
}

template<typename T,
         typename std::enable_if<std::is_convertible<T, CString>::value,
                                 bool>::type* = nullptr,
         typename std::enable_if<!std::is_pointer<T>::value,
                                 bool>::type* = nullptr>
inline CString to_string(T const& value)
{
    if(value == nullptr)
        return "0x0";
    return static_cast<CString>(value);
}

inline CString to_string(const scalar& v)
{
    CString out = Convert::scalarftostring(v);
    return StrUtil::zerortrim(out);
}
inline CString to_string(const bigscalar& v)
{
    CString out = Convert::scalartostring(v);
    return StrUtil::zerortrim(out);
}

//template<typename T>
//inline CString to_string(const T& v)
//{
//    return typeid(v).name();
//}

template<typename... Arg> CString cStringFormat(cstring fmt, Arg... args);

inline CString cStringResolve(CString const& fmt, size_t)
{
    return fmt;
}

extern CString extArgReplace(const CString &fmt, const size_t &index,
                             const CString &replace);

extern CString extArgReplacePhrase(const CString &fmt,
                                   const CString &phrase,
                                   const CString &replace);

}
}

#endif
