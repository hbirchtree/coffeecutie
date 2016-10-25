#ifndef COFFEE_CORE_BASE_DEBUG_EXT_RESOLVERS_H
#define COFFEE_CORE_BASE_DEBUG_EXT_RESOLVERS_H

#include "../../plat/memory/stlstring_ops.h"
#include "../../coffee_mem_macros.h"
#include "../../plat/plat_quirks_toggling.h"
#include "../textprocessing/cregex.h"
#include "../../plat/plat_memory.h"
#include "../../types/composite_types.h"
#include "../../types/vector_types.h"

namespace Coffee{
namespace DebugFun{
namespace conversion{

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
    return "0x"+StrUtil::hexify((uintptr)v,true);
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
//#ifdef COFFEE_ARCH_LLP64
//inline CString to_string(const long int& v)
//{
//    return Convert::inttostring(v);
//}
//#endif

template<typename T>
inline CString to_string(const T& v)
{
    return "NUP";
}

}

template<typename... Arg> CString cStringFormat(cstring fmt, Arg... args);

inline CString cStringResolve(CString const& fmt, size_t)
{
    return fmt;
}

inline CString extArgReplace(CString const& fmt,
                                   size_t const& index,
                                   CString const& replace)
{
    CString subfmt = "{" + Convert::uinttostring(index) + "}";
    return CStrReplace(fmt,subfmt,replace);
}

inline CString extArgReplacePhrase(const CString& fmt,
                                         const CString& phrase,
                                         const CString& replace)
{
    return CStrReplace(fmt,phrase,replace);
}

template<typename T>
inline CString cStringReplace(
        CString const& fmt, size_t const& index,
        const T* const& ptr)
{
    return extArgReplace(fmt,index,conversion::to_string(ptr));
}

inline CString cStringReplace(
        CString const& fmt, size_t const& index,
        cstring arg)
{
    return extArgReplace(fmt,index,(arg) ? arg : "0x0");
}

inline CString cStringReplace(
        CString const& fmt, size_t const& index,
        char* const arg)
{
    return extArgReplace(fmt,index,(arg) ? arg : "0x0");
}

inline CString cStringReplace(
        CString const& fmt, size_t const& index,
        bool const& arg)
{
    return extArgReplace(fmt,index,(arg) ? "true" : "false");
}

inline CString cStringReplace(
        CString const& fmt, size_t const& index,
        const CString& arg)
{
    return extArgReplace(fmt,index,arg);
}

inline CString cStringReplace(
        CString const& fmt, size_t const& index,
        bigscalar const& arg)
{
    /* Regexes, man, these fucking regexes */
    Regex::Pattern patt = Regex::Compile(".*?(\\{\\d+:(\\d+)\\}).*");
    auto match = Regex::Match(patt,fmt,true);

    if(match.size()>=3)
    {
        int32 prec = Convert::strtoint(match[2].s_match[0].c_str());

        CString rep;
#ifdef COFFEE_USE_IOSTREAMS
        std::stringstream ss;
        ss << std::fixed << std::setprecision(prec) << arg;
        ss >> rep;
#else
        rep = Convert::scalartostring(arg);
        cstring dot1 = Search::ChrFind(rep.c_str(),'.');
        cstring dot2 = Search::ChrFind(rep.c_str(),',');
        cstring ptr_start = rep.c_str();
        cstring ptr_end = rep.c_str()+rep.size();
        if(dot1)
            rep.resize(dot1-ptr_start+1+prec,'0');
        else if(dot2)
            rep.resize(dot2-ptr_start+1+prec,'0');
#endif

        return extArgReplacePhrase(fmt,match[1].s_match[0],rep);
    }
    else
        return extArgReplace(fmt,index,conversion::to_string(arg));
}

inline CString cStringReplace(
        CString const& fmt, size_t const& index,
        scalar const& arg)
{
    return cStringReplace(fmt,index,(bigscalar)arg);
}

}
}

#endif

#include "information_extensions.h"
#include "vector_print_extensions.h"
