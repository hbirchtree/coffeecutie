#include <coffee/core/base/debug/information_extensions.h>

#include <coffee/core/base/debug/cdebug_print.h>
#include <coffee/core/types/vector_types.h>

namespace Coffee{
namespace DebugFun{

static cstring cStringify(const Severity &sev)
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
    case Severity::Verbose:
        return "Verbose";
    }
    return "";
}

static cstring cStringify(const DebugType &type)
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

static cstring cStringify(const DebugComponent &comp)
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

CString cStringReplace(const CString &fmt, const size_t &index, const DebugComponent &arg)
{
    return extArgReplace(fmt,index,cStringify(arg));
}

CString cStringReplace(const CString &fmt, const size_t &index, const DebugType &arg)
{
    return extArgReplace(fmt,index,cStringify(arg));
}

CString cStringReplace(const CString &fmt, const size_t &index, const Severity &arg)
{
    return extArgReplace(fmt,index,cStringify(arg));
}

CString cStringReplace(const CString &fmt, const size_t &index, const _cbasic_version<uint8> &arg)
{
    return extArgReplace(fmt,index,cStringFormat("{0}.{1}.{2}",
                                                 arg.major,arg.minor,
                                                 arg.revision));
}

CString cStringReplace(const CString &fmt, const size_t &index, const _cbasic_version<int32> &arg)
{
    return extArgReplace(fmt,index,cStringFormat("{0}.{1}.{2}",
                                                 arg.major,arg.minor,
                                                 arg.revision));
}

CString cStringReplace(const CString &fmt, const size_t &index, const _cbasic_version<uint32> &arg)
{
    return extArgReplace(fmt,index,cStringFormat("{0}.{1}.{2}",
                                                 arg.major,arg.minor,
                                                 arg.revision));
}

CString cStringReplace(const CString &fmt, const size_t &index, const HWDeviceInfo &arg)
{
    if(arg.serial.size())
        return extArgReplace(fmt,index,cStringFormat("{0} {1} running {2}, identified by {3}",
                                                     arg.manufacturer,
                                                     arg.model,
                                                     arg.firmware,
                                                     arg.serial));
    else if(arg.firmware.size())
        return extArgReplace(fmt,index,cStringFormat("{0} {1} running {2}",
                                                     arg.manufacturer,
                                                     arg.model,
                                                     arg.firmware));
    else
        return extArgReplace(fmt,index,cStringFormat("{0} {1}",
                                                     arg.manufacturer,
                                                     arg.model));
}

CString cStringReplace(const CString &fmt, const size_t &index, const SWVersionInfo &arg)
{
    return extArgReplace(fmt,index,
                         cStringFormat("sw={0}, ver={1}.{2}.{3}.{4}.{5}",
                                       arg.name,
                                       arg.product,arg.major,arg.minor,
                                       arg.patch,arg.build));
}

CString cStringReplace(const CString &fmt, const size_t &index, const Display::CGLVersion &arg)
{
    return extArgReplace(fmt,index,cStringFormat("{0}.{1} {2}",
                                                 arg.major,arg.minor,arg.driver));
}

CString cStringReplace(
        CString const& fmt, size_t const& index,
        CQuat const& v)
{
    return extArgReplace(fmt,index,cStringFormat("q({0}:{1},{2},{3})",
                                               v.w(),v.x(),v.y(),v.z()));
}

CString cStringReplace(
        CString const& fmt, size_t const& index,
        CVec4 const& v)
{
    return extArgReplace(fmt,index,cStringFormat("v({0},{1},{2},{3})",
                                               v.x(),v.y(),v.z(),v.w()));
}

CString cStringReplace(
        CString const& fmt, size_t const& index,
        CVec3 const& v)
{
    return extArgReplace(fmt,index,cStringFormat("v({0},{1},{2})",
                                               v.x(),v.y(),v.z()));
}

CString cStringReplace(
        CString const& fmt, size_t const& index,
        CVec2 const& v)
{
    return extArgReplace(fmt,index,cStringFormat("v({0},{1})",
                                               v.x(),v.y()));
}

CString cStringReplace(
        CString const& fmt, size_t const& index,
        CMat4 const& v)
{
    return extArgReplace(
                fmt,index,
                cStringFormat("m({0},{1},{2},{3}})",
                              v[0],v[1],v[2],v[3]));
}

CString cStringReplace(
        CString const& fmt, size_t const& index,
        CMat3 const& v)
{
    return extArgReplace(
                fmt,index,
                cStringFormat("m({0},{1},{2}})",
                              v[0],v[1],v[2]));
}

CString cStringReplace(
    CString const& fmt, size_t const& index,
    BoundBox const& box)
{
    return extArgReplace(
        fmt,index,
        cStringFormat(
            "b({0}->{1},{2}->{3},{4}->{5})",
            box.x1(),box.x2(),
            box.y1(),box.y2(),
                    box.z1(),box.z2()));
}

CString cStringReplace(const CString &fmt, const size_t &index, const Display::CDContextBits &arg)
{
    return extArgReplace(
                fmt,index,
                cStringFormat(
                    "bits(rgba:{0},{1},{2},{3};ds:{4},{5},smp:{6})",
                    arg.red, arg.green, arg.blue, arg.alpha,
                    arg.depth, arg.stencil, arg.samples
                    ));
}

}
}
