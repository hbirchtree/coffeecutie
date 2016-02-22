#ifndef COFFEE_CORE_BASE_DEBUG_EXT_RESOLVERS_H
#define COFFEE_CORE_BASE_DEBUG_EXT_RESOLVERS_H

#include <iostream>
#include <iomanip>
#include <sstream>

#include "../../coffee_macros.h"
#include "../cregex.h"
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
//    CString rep;
//    std::stringstream ss;
//    ss << (T)v;
//    ss >> rep;
//    return rep;
    /* It's broken! */
    return "";
}
#endif

}

template<typename... Arg> CString cStringFormat(cstring fmt, Arg... args);

FORCEDINLINE CString cStringResolve(CString fmt, size_t)
{
    return fmt;
}

FORCEDINLINE CString extArgReplace(const CString& fmt,
                                   size_t index,
                                   const CString& replace)
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

FORCEDINLINE cstring cStringify(DebugComponent comp)
{
    switch(comp)
    {
    case DebugComponent::Audio:
        return "Debug::Audio";
    case DebugComponent::Core:
        return "Debug::Core";
    case DebugComponent::Extension:
        return "Debug::Extension";
    case DebugComponent::GraphicsAPI:
        return "Debug::GraphicsAPI";
    case DebugComponent::Interface:
        return "Debug::Interface";
    case DebugComponent::LibraryLoader:
        return "Debug::LibraryLoader";
    case DebugComponent::Media:
        return "Debug::Media";
    case DebugComponent::ShaderCompiler:
        return "Debug::ShaderCompiler";
    }
    return "";
}
FORCEDINLINE cstring cStringify(DebugType type)
{
    switch(type)
    {
    case DebugType::Compatibility:
        return "Compatibility";
    case DebugType::Compliance:
        return "Compliance";
    case DebugType::Deprecated:
        return "Deprecated";
    case DebugType::Information:
        return "Information";
    case DebugType::Marker:
        return "Marker";
    case DebugType::Performance:
        return "Performance";
    case DebugType::UndefinedBehavior:
        return "Undefined behavior";
    case DebugType::Other:
        return "Other";
    }
    return "";
}
FORCEDINLINE cstring cStringify(Severity sev)
{
    switch(sev)
    {
    case Severity::Critical:
        return "Critical";
    case Severity::Fatal:
        return "Fatal";
    case Severity::Debug:
        return "Debug";
    case Severity::High:
        return "High";
    case Severity::Medium:
        return "Medium";
    case Severity::Low:
        return "Low";
    case Severity::Information:
        return "Information";
    }
    return "";
}

/* Extension resolvers */

FORCEDINLINE CString cStringReplace(
        CString fmt, size_t index,
        DebugComponent arg)
{
    return extArgReplace(fmt,index,cStringify(arg));
}

FORCEDINLINE CString cStringReplace(
        CString fmt, size_t index,
        DebugType arg)
{
    return extArgReplace(fmt,index,cStringify(arg));
}

FORCEDINLINE CString cStringReplace(
        CString fmt, size_t index,
        Severity arg)
{
    return extArgReplace(fmt,index,cStringify(arg));
}

FORCEDINLINE CString cStringReplace(
        CString fmt, size_t index,
        const _cbasic_version<uint8>& arg)
{
    return extArgReplace(fmt,index,cStringFormat("{0}.{1}.{2}",
                                               arg.major,arg.minor,
                                               arg.revision));
}

FORCEDINLINE CString cStringReplace(
        CString fmt, size_t index,
        const _cbasic_version<int32>& arg)
{
    return extArgReplace(fmt,index,cStringFormat("{0}.{1}.{2}",
                                               arg.major,arg.minor,
                                               arg.revision));
}

FORCEDINLINE CString cStringReplace(
        CString fmt, size_t index,
        const _cbasic_version<uint32>& arg)
{
    return extArgReplace(fmt,index,cStringFormat("{0}.{1}.{2}",
                                               arg.major,arg.minor,
                                               arg.revision));
}

FORCEDINLINE CString cStringReplace(
        CString fmt, size_t index,
        HWDeviceInfo const& arg)
{
    return extArgReplace(fmt,index,cStringFormat("mf={0}, md={1}, fw={2}",
                                               arg.manufacturer,
                                               arg.model,
                                               arg.firmware));
}

FORCEDINLINE CString cStringReplace(
        CString fmt, size_t index,
        CQuat const& v)
{
    return extArgReplace(fmt,index,cStringFormat("q({0}:{1},{2},{3})",
                                               v.w(),v.x(),v.y(),v.z()));
}

FORCEDINLINE CString cStringReplace(
        CString fmt, size_t index,
        CVec4 const& v)
{
    return extArgReplace(fmt,index,cStringFormat("v({0},{1},{2},{3})",
                                               v.x(),v.y(),v.z(),v.w()));
}

FORCEDINLINE CString cStringReplace(
        CString fmt, size_t index,
        CVec3 const& v)
{
    return extArgReplace(fmt,index,cStringFormat("v({0},{1},{2})",
                                               v.x(),v.y(),v.z()));
}

FORCEDINLINE CString cStringReplace(
        CString fmt, size_t index,
        CVec2 const& v)
{
    return extArgReplace(fmt,index,cStringFormat("v({0},{1})",
                                               v.x(),v.y()));
}

FORCEDINLINE CString cStringReplace(
        CString fmt, size_t index,
        CMat4 const& v)
{
    return extArgReplace(
                fmt,index,
                cStringFormat("m({0},{1},{2},{3}})",
                              v[0],v[1],v[2],v[3]));
}

FORCEDINLINE CString cStringReplace(
        CString fmt, size_t index,
        CMat3 const& v)
{
    return extArgReplace(
                fmt,index,
                cStringFormat("m({0},{1},{2}})",
                              v[0],v[1],v[2]));
}

template<typename T>
FORCEDINLINE CString cStringReplace(
        CString fmt, size_t index,
        const T* const& ptr)
{
    CString rep;
    std::stringstream ss;
    ss << std::hex << (uintptr_t)ptr;
    ss >> rep;
    return extArgReplace(fmt,index,"0x"+rep);
}

FORCEDINLINE CString cStringReplace(
        CString fmt, size_t index,
        cstring arg)
{
    return extArgReplace(fmt,index,(arg) ? arg : "0x0");
}

FORCEDINLINE CString cStringReplace(
        CString fmt, size_t index,
        char* const arg)
{
    return extArgReplace(fmt,index,(arg) ? arg : "0x0");
}

FORCEDINLINE CString cStringReplace(
        CString fmt, size_t index,
        bool const& arg)
{
    return extArgReplace(fmt,index,(arg) ? "true" : "false");
}

FORCEDINLINE CString cStringReplace(
        CString fmt, size_t index,
        const CString& arg)
{
    return extArgReplace(fmt,index,arg);
}

FORCEDINLINE CString cStringReplace(
        CString fmt, size_t index,
        const bigscalar& arg)
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

}
}

#endif