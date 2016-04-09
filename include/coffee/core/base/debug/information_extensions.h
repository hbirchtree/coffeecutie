#pragma once

#include "extensionresolvers.h"

#include "../types/cdisplay.h"

namespace Coffee{
namespace DebugFun{

inline cstring cStringify(DebugComponent const& comp)
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
inline cstring cStringify(DebugType const& type)
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
inline cstring cStringify(Severity const& sev)
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

inline CString cStringReplace(
        CString const& fmt, size_t const& index,
        DebugComponent const& arg)
{
    return extArgReplace(fmt,index,cStringify(arg));
}

inline CString cStringReplace(
        CString const& fmt, size_t const& index,
        DebugType const& arg)
{
    return extArgReplace(fmt,index,cStringify(arg));
}

inline CString cStringReplace(
        CString const& fmt, size_t const& index,
        Severity const& arg)
{
    return extArgReplace(fmt,index,cStringify(arg));
}

inline CString cStringReplace(
        CString const& fmt, size_t const& index,
        const _cbasic_version<uint8>& arg)
{
    return extArgReplace(fmt,index,cStringFormat("{0}.{1}.{2}",
                                               arg.major,arg.minor,
                                               arg.revision));
}

inline CString cStringReplace(
        CString const& fmt, size_t const& index,
        const _cbasic_version<int32>& arg)
{
    return extArgReplace(fmt,index,cStringFormat("{0}.{1}.{2}",
                                               arg.major,arg.minor,
                                               arg.revision));
}

inline CString cStringReplace(
        CString const& fmt, size_t const& index,
        const _cbasic_version<uint32>& arg)
{
    return extArgReplace(fmt,index,cStringFormat("{0}.{1}.{2}",
                                               arg.major,arg.minor,
                                               arg.revision));
}

inline CString cStringReplace(
        CString const& fmt, size_t const& index,
        HWDeviceInfo const& arg)
{
    return extArgReplace(fmt,index,cStringFormat("mf={0}, md={1}, fw={2}",
                                               arg.manufacturer,
                                               arg.model,
                                               arg.firmware));
}

inline CString cStringReplace(
        CString const& fmt, size_t const& index,
        SWVersionInfo const& arg)
{
    return extArgReplace(fmt,index,
                         cStringFormat("sw={0}, ver={1}.{2}.{3}.{4}.{5}",
                                       arg.name,
                                       arg.product,arg.major,arg.minor,
                                       arg.patch,arg.build));
}

inline CString cStringReplace(
        CString const& fmt, size_t const& index,
        Display::CGLVersion const& arg)
{
    return extArgReplace(fmt,index,cStringFormat("major={0}, minor={1}, extra={2}",
                                                 arg.major,arg.minor,arg.driver));
}

template<typename T>
inline CString cStringReplace(
        CString const& fmt, size_t const& index,
        _cbasic_point<T> v)
{
    return extArgReplace(fmt,index,cStringFormat("p({0},{1})",
                                               v.x,v.y));
}

template<typename T>
inline CString cStringReplace(
        CString const& fmt, size_t const& index,
        _cbasic_size_2d<T> v)
{
    return extArgReplace(fmt,index,cStringFormat("sz({0},{1})",
                                               v.w,v.h));
}

}
}
