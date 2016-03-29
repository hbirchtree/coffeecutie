#ifndef COFFEE_CORE_BASE_DEBUG_EXT_RESOLVERS_H
#define COFFEE_CORE_BASE_DEBUG_EXT_RESOLVERS_H

#include "../../coffee_macros.h"
#include "../textprocessing/cregex.h"
#include "../../plat/plat_memory.h"
#include "../../types/composite_types.h"
#include "../../types/vector_types.h"

namespace Coffee{
namespace DebugFun{
namespace conversion{

#if defined(COFFEE_USE_STL_TO_STRING)
using namespace std;
#else
template<typename T>
FORCEDINLINE std::string to_string(const T& v)
{
    CString rep;
    std::stringstream ss;
    ss << v;
    ss >> rep;
    return rep;
}
#endif

}

template<typename... Arg> CString cStringFormat(cstring fmt, Arg... args);

FORCEDINLINE CString cStringResolve(CString const& fmt, size_t)
{
    return fmt;
}

FORCEDINLINE CString extArgReplace(CString const& fmt,
                                   size_t const& index,
                                   CString const& replace)
{
    CString subfmt = "{" + conversion::to_string(index) + "}";
    return CStrReplace(fmt,subfmt,replace);
}

FORCEDINLINE CString extArgReplacePhrase(const CString& fmt,
                                         const CString& phrase,
                                         const CString& replace)
{
    return CStrReplace(fmt,phrase,replace);
}

template<typename T>
FORCEDINLINE CString cStringReplace(
        CString const& fmt, size_t const& index,
        const T* const& ptr)
{
    CString rep;
    std::stringstream ss;
    ss << std::hex << (uintptr_t)ptr;
    ss >> rep;
    return extArgReplace(fmt,index,"0x"+rep);
}

FORCEDINLINE CString cStringReplace(
        CString const& fmt, size_t const& index,
        cstring arg)
{
    return extArgReplace(fmt,index,(arg) ? arg : "0x0");
}

FORCEDINLINE CString cStringReplace(
        CString const& fmt, size_t const& index,
        char* const arg)
{
    return extArgReplace(fmt,index,(arg) ? arg : "0x0");
}

FORCEDINLINE CString cStringReplace(
        CString const& fmt, size_t const& index,
        bool const& arg)
{
    return extArgReplace(fmt,index,(arg) ? "true" : "false");
}

FORCEDINLINE CString cStringReplace(
        CString const& fmt, size_t const& index,
        const CString& arg)
{
    return extArgReplace(fmt,index,arg);
}

FORCEDINLINE CString cStringReplace(
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
        std::stringstream ss;
        ss << std::fixed << std::setprecision(prec) << arg;
        ss >> rep;

        return extArgReplacePhrase(fmt,match[1].s_match[0],rep);
    }
    else
        return extArgReplace(fmt,index,conversion::to_string(arg));
}

FORCEDINLINE CString cStringReplace(
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