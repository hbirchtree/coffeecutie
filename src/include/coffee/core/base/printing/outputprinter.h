#pragma once

#include "../debug/debug_interface.h"
#include "../strings/cdebug_print.h"
#include <coffee/core/plat/memory/cmemory.h>
#include <coffee/core/plat/plat_environment.h>

namespace Coffee {

extern u8& PrintingVerbosityLevel();

namespace DebugFun {

using namespace Strings;

extern cstring severity_string(Severity sev);

using LogInterfaceBasic = void (*)(FILE*, CString const&, Severity, u32, u32);
using LogInterfaceTagged =
    void (*)(FILE*, cstring, CString const&, Severity, u32, u32);

struct LogInterface
{
    LogInterfaceBasic  basic;
    LogInterfaceTagged tag;
};

/*!
 * \brief Get the currently set logging interface
 * By default this is set to OutputPrinterImpl::fprintf_platform
 * \return
 */
extern LogInterface GetLogInterface();
extern void         SetLogInterface(LogInterface intf);

struct OutputPrinterImpl
{
    enum PrintingFlags
    {
        Flag_None      = 0,
        Flag_NoContext = 0x1,
        Flag_NoNewline = 0x2,
    };

    using CmdColor = ColorMap::CmdColor;

    static Mutex PrinterLock;

    static void fprintf_platform_tagged(
        FILE*          stream,
        cstring        tag,
        CString const& formatted_raw,
        Severity       sev,
        u32            level,
        u32            flags);

    STATICINLINE void fprintf_platform(
        FILE*          stream,
        CString const& formatted_raw,
        Severity       sev,
        u32            level,
        u32            flags)
    {
        fprintf_platform_tagged(
            stream, nullptr, formatted_raw, sev, level, flags);
    }

    template<typename... Args>
    static void fprintf(
        FILE*    stream,
        Severity sev,
        u32      level,
        u32      flags,
        cstring  format,
        Args... args)
    {
        CString formatted = Strings::cStringFormat(format, args...);
        GetLogInterface().basic(stream, &formatted[0], sev, level, flags);
    }

    template<typename... Args>
    static void fprintf_tagged(
        FILE*    stream,
        Severity sev,
        u32      level,
        u32      flags,
        cstring  tag,
        cstring  format,
        Args... args)
    {
        CString formatted = Strings::cStringFormat(format, args...);
        GetLogInterface().tag(
            stream, tag, formatted.c_str(), sev, level, flags);
    }
};

using OutputPrinter = OutputPrinterImpl;

} // namespace DebugFun
} // namespace Coffee
