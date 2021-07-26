#include <platforms/base/sysinfo.h>
#include <platforms/stacktrace.h>

#include <platforms/pimpl_state.h>

#include <peripherals/stl/functional_types.h>
#include <peripherals/stl/stlstring_ops.h>

#include <peripherals/stl/string_casting.h>

#if defined(COFFEE_UNIXPLAT)
#include <unistd.h>
#elif defined(COFFEE_WINDOWS)
#include <peripherals/platform/windows.h>
#endif

namespace platform::env {

CString SysInfoDef::GetSystemString()
{
    /* Please don't ask about the leading space :) */
    const constexpr cstring _fmt = "%.3s%u_%s ";
    int                     len =
        snprintf(nullptr, 0, _fmt, C_SYSTEM_STRING, BitNess(), COFFEE_ARCH);
    CString base;
    base.resize(len);
    snprintf(
        &base[0], base.size(), _fmt, C_SYSTEM_STRING, BitNess(), COFFEE_ARCH);
    base.resize(base.find('\0'));
    base = str::fmt::lower(base);
    return base;
}

CString SysInfoDef::HostName()
{
#if MODE_DEBUG

#if defined(COFFEE_UNIXPLAT) || defined(COFFEE_WINDOWS)
    /* For now, we assume this works. We might implement a better one where it
     * retries upon failure. */
    CString _m;
    _m.resize(60);
    gethostname(&_m[0], _m.size());
    _m.resize(_m.find('\0'));
    return _m;
#else
    return "localhost";
#endif

#else
    return {};
#endif
}

#if !defined(COFFEE_LINUX) || !defined(COFFEE_WINDOWS)
info::HardwareDevice SysInfoDef::Processor(C_UNUSED(u32 i))
{
    return info::HardwareDevice(COFFEE_ARCH, "0x0");
}
#endif

#if !defined(COFFEE_LINUX) || !defined(COFFEE_ANDROID)
info::HardwareDevice SysInfoDef::DeviceName()
{
    return info::HardwareDevice(C_SYSTEM_STRING, "0x0");
}

info::HardwareDevice SysInfoDef::Motherboard()
{
    return info::HardwareDevice(C_SYSTEM_STRING, "0x0");
}

info::HardwareDevice SysInfoDef::Chassis()
{
    return info::HardwareDevice(C_SYSTEM_STRING, "0x0");
}

info::HardwareDevice SysInfoDef::BIOS()
{
    return info::HardwareDevice(C_SYSTEM_STRING, "0x0");
}
#endif

} // namespace platform::env

namespace platform::stacktrace {

void print_frames(
    stacktrace&&                       frames,
    typing::logging::LogInterfaceBasic print,
    typing::logging::StackWriterEx     stack_writer)
{
    detail::print(print, "dumping stacktrace:");
    for(auto const& frame : frames)
    {
        // clang-format off
        detail::print(
            print,
            stl_types::String(" >> ") +
            "exec(" + frame.name() + ") [0x????] " +
            (frame.source_file().empty() ?
                stl_types::String() :
                frame.source_file() + ":" + cast_pod(frame.source_line()))
        );
        // clang-format on
    }
}

void print_exception(
    stl_types::Pair<std::string, stacktrace>&& exception,
    typing::logging::LogInterfaceBasic         print,
    typing::logging::StackWriterEx             stack_writer)
{
    detail::print(print, "exception encountered:");
    detail::print(print, " >> " + exception.first);

    print_frames(std::move(exception.second), print, stack_writer);
}

} // namespace platform::stacktrace
