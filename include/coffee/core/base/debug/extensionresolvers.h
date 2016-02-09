#ifndef COFFEE_CORE_BASE_DEBUG_EXT_RESOLVERS_H
#define COFFEE_CORE_BASE_DEBUG_EXT_RESOLVERS_H

#include <iostream>
#include <iomanip>
#include <sstream>

#include "../../coffee_macros.h"
#include "../../plat/plat_memory.h"
#include "../../types/types.h"

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

FORCEDINLINE CString cStrReplace(const CString& fmt,
                                          size_t index,
                                          const CString& replace)
{
    CString subfmt = "{" + conversion::to_string(index) + "}";
    return CStrReplace(fmt,subfmt,replace);
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
    return cStrReplace(fmt,index,cStringify(arg));
}

FORCEDINLINE CString cStringReplace(
        CString fmt, size_t index,
        DebugType arg)
{
    return cStrReplace(fmt,index,cStringify(arg));
}

FORCEDINLINE CString cStringReplace(
        CString fmt, size_t index,
        Severity arg)
{
    return cStrReplace(fmt,index,cStringify(arg));
}

FORCEDINLINE CString cStringReplace(
        CString fmt, size_t index,
        const _cbasic_version<uint8>& arg)
{
    return cStrReplace(fmt,index,cStringFormat("{0}.{1}.{2}",
                                               arg.major,arg.minor,
                                               arg.revision));
}

FORCEDINLINE CString cStringReplace(
        CString fmt, size_t index,
        const _cbasic_version<int32>& arg)
{
    return cStrReplace(fmt,index,cStringFormat("{0}.{1}.{2}",
                                               arg.major,arg.minor,
                                               arg.revision));
}

FORCEDINLINE CString cStringReplace(
        CString fmt, size_t index,
        const _cbasic_version<uint32>& arg)
{
    return cStrReplace(fmt,index,cStringFormat("{0}.{1}.{2}",
                                               arg.major,arg.minor,
                                               arg.revision));
}

FORCEDINLINE CString cStringReplace(
        CString fmt, size_t index,
        HWDeviceInfo const& arg)
{
    return cStrReplace(fmt,index,cStringFormat("mf={0}, md={1}, fw={2}",
                                               arg.manufacturer,
                                               arg.model,
                                               arg.firmware));
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
    return cStrReplace(fmt,index,"0x"+rep);
}

FORCEDINLINE CString cStringReplace(
        CString fmt, size_t index,
        cstring arg)
{
    return cStrReplace(fmt,index,(arg) ? arg : "0x0");
}

FORCEDINLINE CString cStringReplace(
        CString fmt, size_t index,
        char* const arg)
{
    return cStrReplace(fmt,index,(arg) ? arg : "0x0");
}

FORCEDINLINE CString cStringReplace(
        CString fmt, size_t index,
        bool const& arg)
{
    return cStrReplace(fmt,index,(arg) ? "true" : "false");
}

FORCEDINLINE CString cStringReplace(
        CString fmt, size_t index,
        const CString& arg)
{
    return cStrReplace(fmt,index,arg);
}

}
}

#endif