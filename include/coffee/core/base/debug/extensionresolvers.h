#ifndef COFFEE_CORE_BASE_DEBUG_EXT_RESOLVERS_H
#define COFFEE_CORE_BASE_DEBUG_EXT_RESOLVERS_H

#include "../../plat/memory/stlstring_ops.h"
#include "../../coffee_mem_macros.h"
#include "../../plat/plat_quirks_toggling.h"
#include "../../plat/memory/stlstring_ops.h"

namespace Coffee{
namespace DebugFun{
namespace conversion{

using namespace Mem;

inline CString to_string(const char* const& v)
{
    return CString(v);
}
inline CString to_string(const CString& v)
{
    return v;
}
template<typename T>
inline CString to_string(const T* const& v)
{
    return "0x"+StrUtil::hexify(C_FCAST<uintptr>(v),true);
}

inline CString to_string(const uint8& v)
{
    return Convert::uintctostring(v);
}
inline CString to_string(const int8& v)
{
    return Convert::intctostring(v);
}
inline CString to_string(const uint16& v)
{
    return Convert::uintstostring(v);
}
inline CString to_string(const int16& v)
{
    return Convert::intstostring(v);
}
inline CString to_string(const uint32& v)
{
    return Convert::uinttostring(v);
}
inline CString to_string(const int32& v)
{
    return Convert::inttostring((int32)v);
}
inline CString to_string(const uint64& v)
{
    return Convert::uintltostring(v);
}
inline CString to_string(const int64& v)
{
    return Convert::intltostring((int64)v);
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

template<typename T>
inline CString to_string(const T& v)
{
    return typeid(v).name();
}

}

template<typename... Arg> CString cStringFormat(cstring fmt, Arg... args);

inline CString cStringResolve(CString const& fmt, size_t)
{
    return fmt;
}

extern CString extArgReplace(CString const& fmt,
                                   size_t const& index,
                                   CString const& replace);

extern CString extArgReplacePhrase(const CString& fmt,
                                         const CString& phrase,
                                         const CString& replace);

template<typename T>
inline CString cStringReplace(
        CString const& fmt, size_t const& index,
        const T* const& ptr)
{
    return extArgReplace(fmt,index,conversion::to_string(ptr));
}

extern CString cStringReplace(
        CString const& fmt, size_t const& index,
        cstring arg);

extern CString cStringReplace(
        CString const& fmt, size_t const& index,
        char* const arg);

extern CString cStringReplace(
        CString const& fmt, size_t const& index,
        bool const& arg);

extern CString cStringReplace(
        CString const& fmt, size_t const& index,
        const CString& arg);

extern CString cStringReplace(
        CString const& fmt, size_t const& index,
        bigscalar const& arg);

extern CString cStringReplace(
        CString const& fmt, size_t const& index,
        scalar const& arg);

template<typename T>
FORCEDINLINE CString cStringReplace(
        CString const& fmt, size_t const& index,
        const T& arg)
{
    return extArgReplace(fmt,index,conversion::to_string(arg));
}

}
}

#endif
