#include <coffee/core/base/strings/information_extensions.h>
#include <coffee/core/types/vector_types.h>

#include <coffee/core/base/strings/string_format.h>

namespace Coffee {
namespace Strings {

cstring to_string(const Severity& sev)
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

cstring to_string(const DebugType& type)
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

cstring to_string(const DebugComponent& comp)
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

template<typename T>
static CString version_to_string(_cbasic_version<T> const& arg)
{
    return cast_pod(arg.major) + '.' + cast_pod(arg.minor) + '.' +
           cast_pod(arg.revision);
}

CString to_string(const _cbasic_version<u8>& arg)
{
    return version_to_string<u8>(arg);
}

CString to_string(const _cbasic_version<i32>& arg)
{
    return version_to_string<i32>(arg);
}

CString to_string(const _cbasic_version<u32>& arg)
{
    return version_to_string<u32>(arg);
}

void conditional_add(CString& src, bool add_space)
{
    if(add_space)
        src += ' ';
}

CString to_string(const HWDeviceInfo& arg)
{
    CString output;

    if(arg.manufacturer.size())
    {
        conditional_add(output, output.size());
        output += arg.manufacturer;
    }

    if(arg.model.size())
    {
        conditional_add(output, output.size());
        output += arg.model;
    }

    if(arg.firmware.size())
    {
        conditional_add(output, output.size());
        output += "running " + arg.firmware;
    }

    if(arg.serial.size())
    {
        conditional_add(output, output.size());
        output += "identified by " + arg.serial;
    }

    return output;
}

CString to_string(const SWVersionInfo& arg)
{
    return cStringFormat(
        "sw={0}, ver={1}.{2}.{3}.{4}.{5}",
        arg.name,
        arg.product,
        arg.major,
        arg.minor,
        arg.patch,
        arg.build);
}

CString to_string(const Display::CGLVersion& arg)
{
    return cStringFormat("{0}.{1} {2}", arg.major, arg.minor, arg.driver);
}

CString to_string(BoundBox const& box)
{
    return cStringFormat(
        "b({0}->{1},{2}->{3},{4}->{5})",
        box.x1(),
        box.x2(),
        box.y1(),
        box.y2(),
        box.z1(),
        box.z2());
}

CString to_string(const Display::CDContextBits& arg)
{
    return cStringFormat(
        "bits(rgba:{0},{1},{2},{3};ds:{4},{5},smp:{6})",
        arg.red,
        arg.green,
        arg.blue,
        arg.alpha,
        arg.depth,
        arg.stencil,
        arg.samples);
}

CString to_string(Display::CDMonitor const& arg)
{
    return cStringFormat(
        "monitor(rgbx:{0},{1},{2},{3})",
        arg.colorBits.red,
        arg.colorBits.green,
        arg.colorBits.blue,
        arg.colorBits.extra);
}

} // namespace Strings
} // namespace Coffee
