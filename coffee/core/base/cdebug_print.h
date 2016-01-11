#ifndef COFFEE_CORE_DEBUG_PRINT_H
#define COFFEE_CORE_DEBUG_PRINT_H

#include <iomanip>
#include <sstream>
#include "coffee/core/coffee_macros.h"
#include "coffee/core/plat/cmemory.h"
#include "coffee/core/types/types.h"

namespace Coffee{
namespace CDebugPrint{

template<typename... Arg> CString cStringFormat(cstring fmt, Arg... args);

inline C_FORCE_INLINE CString cStringResolve(CString fmt, size_t)
{
    return fmt;
}

inline C_FORCE_INLINE CString cStrReplace(CString fmt, size_t index, CString replace)
{
    CString subfmt = "{" + std::to_string(index) + "}";
    return CStrReplace(fmt,subfmt,replace);
}

//TODO: Make these constexpr (C++14)
inline C_FORCE_INLINE cstring cStringify(DebugComponent comp)
{
    switch(comp)
    {
    case DebugComponent::Audio:
        return "Debug::Audio";
    case DebugComponent::Core:
        return "Debug::Core";
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
inline C_FORCE_INLINE cstring cStringify(DebugType type)
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
inline C_FORCE_INLINE cstring cStringify(Severity sev)
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

inline C_FORCE_INLINE CString cStringReplace(
        CString fmt, size_t index,
        DebugComponent arg)
{
    return cStrReplace(fmt,index,cStringify(arg));
}

inline C_FORCE_INLINE CString cStringReplace(
        CString fmt, size_t index,
        DebugType arg)
{
    return cStrReplace(fmt,index,cStringify(arg));
}

inline C_FORCE_INLINE CString cStringReplace(
        CString fmt, size_t index,
        Severity arg)
{
    return cStrReplace(fmt,index,cStringify(arg));
}

inline C_FORCE_INLINE CString cStringReplace(
        CString fmt, size_t index,
        const _cbasic_version<uint8>& arg)
{
    return cStrReplace(fmt,index,cStringFormat("{0}.{1}.{2}",
                                               arg.major,arg.minor,
                                               arg.revision));
}

inline C_FORCE_INLINE CString cStringReplace(
        CString fmt, size_t index,
        HWDeviceInfo const& arg)
{
    return cStrReplace(fmt,index,cStringFormat("{0} {1}",
                                               arg.model,
                                               arg.firmware));
}

/* Core string resolution */

template<typename T>
inline C_FORCE_INLINE CString cStringReplace(
        CString fmt, size_t index,
        const T* const& ptr)
{
    CString rep;
    std::stringstream ss;
    ss << std::hex << (uintptr_t)ptr;
    ss >> rep;
    return cStrReplace(fmt,index,"0x"+rep);
}

inline C_FORCE_INLINE CString cStringReplace(
        CString fmt, size_t index,
        cstring arg)
{
    return cStrReplace(fmt,index,(arg) ? arg : "0x0");
}

inline C_FORCE_INLINE CString cStringReplace(
        CString fmt, size_t index,
        char* const arg)
{
    return cStrReplace(fmt,index,(arg) ? arg : "0x0");
}

inline C_FORCE_INLINE CString cStringReplace(
        CString fmt, size_t index,
        const CString& arg)
{
    return cStrReplace(fmt,index,arg.c_str());
}

template<typename T>
inline C_FORCE_INLINE CString cStringReplace(
        CString fmt, size_t index,
        const T& arg)
{
    return cStrReplace(fmt,index,std::to_string(arg));
}

template<typename T>
inline C_FORCE_INLINE CString cStringResolve(CString fmt, size_t index, const T& arg)
{
    CString str = cStringReplace(fmt,index,arg);
    return cStringResolve(str,++index);
}

template<typename... Args, typename T>
inline C_FORCE_INLINE CString cStringResolve(CString fmt, size_t index, const T& arg, Args... args)
{
    CString str = cStringReplace(fmt,index,arg);
    return cStringResolve(str,++index,args...);
}

template<typename... Arg>
inline C_FORCE_INLINE CString cStringFormat(cstring fmt, Arg... args)
{
    CString str = cStringResolve(fmt,0,args...);
//    str.resize(snprintf(NULL,0,fmt,args...)+1);
//    sprintf(&str[0],fmt,args...);
    return str;
}

}
}

#endif
