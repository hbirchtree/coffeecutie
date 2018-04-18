#ifndef COFFEE_CORE_BASE_DEBUG_DEBUGPRINTER_H
#define COFFEE_CORE_BASE_DEBUG_DEBUGPRINTER_H

#include "../../plat/timing/timing_def.h"
#include "../../plat/plat_environment.h"

#include "debug_interface.h"

#include "../printing/outputprinter.h"

namespace Coffee{

extern u8& PrintingVerbosityLevel();

namespace DebugFun{

using namespace Strings;

extern cstring severity_string(Severity sev);

struct DebugPrinterImpl : DebugPrinterDef
{
    using CmdColor = ColorMap::CmdColor;

#if !defined(COFFEE_EMSCRIPTEN)
    static const constexpr cstring print_fmt = "{0}\n";
#else
    static const constexpr cstring print_fmt = "{0}";
#endif

    static void AddContextString(CString& prefix, Severity sev);

    template<typename... Args>
    STATICINLINE CString FormatPrintString(Severity sev, uint32,
                                           cstring fmt,Args... args)
    {
        CString formatted = cStringFormat(fmt,args...);

#if !defined(COFFEE_PLATFORM_OUTPUT_FORMAT)
        CString prefix;
        AddContextString(prefix, sev);
        return prefix + ": " + formatted;
#else
        return formatted;
#endif
    }

    template<typename... Args>
    STATICINLINE void cDebug(cstring f, Args... a)
    {
        if(PrintingVerbosityLevel() < 1)
            return;

        CString fmt = FormatPrintString(Severity::Debug,
                                        0,f,a...);
        OutputPrinter::fprintf(DefaultDebugOutputPipe,Severity::Debug,
                               print_fmt,fmt);
    }
    template<typename... Args>
    STATICINLINE void cWarning(cstring f, Args... a)
    {
        if(PrintingVerbosityLevel() < 1)
            return;

        CString fmt = FormatPrintString(Severity::Medium,
                                        0,f,a...);
        OutputPrinter::fprintf(DefaultDebugOutputPipe,Severity::Medium,
                               print_fmt,fmt);
    }
    template<typename... Args>
    STATICINLINE void cFatal(cstring f, Args... a)
    {
        CString fmt = FormatPrintString(Severity::Fatal,
                                        0,f,a...);
        OutputPrinter::fprintf(DefaultDebugOutputPipe,Severity::Fatal,
                               print_fmt,fmt);
    }

    template<typename... Args>
    STATICINLINE void cBasicPrint(cstring f, Args... a)
    {
        if(PrintingVerbosityLevel() < 1)
            return;

        CString fmt = cStringFormat(f,a...);
        OutputPrinter::fprintf(DefaultDebugOutputPipe,Severity::Information,
                               print_fmt,fmt);
    }
    template<typename... Args>
    STATICINLINE void cBasicPrintNoNL(cstring f, Args... a)
    {
        if(PrintingVerbosityLevel() < 1)
            return;

        OutputPrinter::fprintf(DefaultDebugOutputPipe,Severity::Information,
                               f,a...);
    }

    template<typename... Args>
    STATICINLINE void cVerbose(uint32 level, cstring f, Args... a)
    {
#ifndef NDEBUG
        if(PrintingVerbosityLevel() < level)
            return;

        CString fmt = FormatPrintString(Severity::Verbose,
                                        0,f,a...);
        OutputPrinter::fprintf(DefaultDebugOutputPipe,Severity::Verbose,
                               print_fmt,fmt);
#endif
    }
};

using DebugPrinter = DebugPrinterImpl;

}
}

#endif
