#pragma once

#include <peripherals/libc/types.h>
#include <peripherals/stl/types.h>

namespace Coffee {
namespace Strings {

template<typename Dummy, typename std::enable_if<false, Dummy>::type* = nullptr>
CString to_string(Dummy const&)
{
    return {};
}

/* Core string resolution */

extern CString extArgReplace(
    const CString& fmt, const size_t& index, const CString& replace);

extern CString extArgReplacePhrase(
    const CString& fmt, const CString& phrase, const CString& replace);

template<typename T>
FORCEDINLINE CString cStringReplace(CString& fmt, size_t index, T const& arg)
{
    return extArgReplace(fmt, index, to_string(arg));
}

template<typename T>
FORCEDINLINE CString cStringResolve(CString& fmt, size_t index, const T& arg)
{
    return cStringReplace(fmt, index, arg);
}

template<typename... Args, typename T>
FORCEDINLINE CString
             cStringResolve(CString& fmt, size_t index, const T& arg, Args... args)
{
    CString str = cStringReplace(fmt, index, arg);
    return cStringResolve(str, ++index, args...);
}

template<typename... Arg>
FORCEDINLINE CString cStringFormat(cstring fmt, Arg... args)
{
    CString fmt_ = fmt;

    fmt_ = cStringResolve(fmt_, 0, args...);

    return fmt_;
}

FORCEDINLINE CString cStringFormat(cstring fmt)
{
    return fmt;
}

template<typename... Arg>
FORCEDINLINE CString fmt(cstring fmt, Arg... arg)
{
    return cStringFormat(fmt, arg...);
}

} // namespace Strings
} // namespace Coffee
