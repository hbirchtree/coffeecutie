#ifndef COFFEE_CORE_BASE_DEBUG_DEBUGPRINTER_H
#define COFFEE_CORE_BASE_DEBUG_DEBUGPRINTER_H

#include "../../plat/timing/timing_def.h"
#include "../../plat/plat_environment.h"

#include "debug_interface.h"

#include "outputprinter.h"

namespace Coffee{

extern uint8 PrintingVerbosityLevel;

namespace DebugFun{

struct DebugPrinterImpl : DebugPrinterDef
{
    using CmdColor = ColorMap::CmdColor;

    static const constexpr cstring print_fmt = "{0}\n";

    template<typename... Args>
    STATICINLINE CString FormatPrintString(Severity sev, uint32 stackoffset,
                                     cstring fmt, Args... args)
    {
        cstring const severity_info = "INFO";
        cstring const severity_debg = "DEBG";
        cstring const severity_warn = "WARN";
        cstring const severity_ftal = "FTAL";
        cstring const severity_verb = "VERB";

        cstring severity_str = nullptr;

        switch(sev)
        {
        case Severity::Information:
            severity_str = severity_info;
            break;
        case Severity::Verbose:
            severity_str = severity_verb;
            break;
        case Severity::Debug:
        case Severity::Low:
            severity_str = severity_debg;
            break;
        case Severity::Medium:
            severity_str = severity_warn;
            break;
        case Severity::Critical:
        case Severity::Fatal:
        case Severity::High:
            severity_str = severity_ftal;
            break;
        }

        CString cclock = Time::ClockString();
#if !defined(COFFEE_ANDROID) && !defined(__EMSCRIPTEN__)
        CString clock = cStringFormat("{0}.{1}",cclock,Time::Microsecond()/1000);
        CString prefix = cStringFormat("{0}:{1}", clock.c_str(), severity_str);
#else
        CString prefix = severity_str[0];
#endif

        ColorMap::ColorText(prefix, ColorMap::CombineFormat(CmdColor::Green, CmdColor::Blue));

        CString formatted = cStringFormat(fmt,args...);

        return cStringFormat("{0}: {1}",prefix,formatted);
    }

    template<typename... Args>
    STATICINLINE void cDebug(cstring f, Args... a)
    {
        if(PrintingVerbosityLevel < 1)
            return;

        CString fmt = FormatPrintString(Severity::Debug,
                                        0,f,a...);
        OutputPrinter::fprintf(DefaultDebugOutputPipe,print_fmt,fmt);
    }
    template<typename... Args>
    STATICINLINE void cWarning(cstring f, Args... a)
    {
        if(PrintingVerbosityLevel < 1)
            return;

        CString fmt = FormatPrintString(Severity::Medium,
                                        0,f,a...);
        OutputPrinter::fprintf(DefaultDebugOutputPipe,print_fmt,fmt);
    }
    template<typename... Args>
    STATICINLINE void cFatal(cstring f, Args... a)
    {
        CString fmt = FormatPrintString(Severity::Fatal,
                                        0,f,a...);
        OutputPrinter::fprintf(DefaultDebugOutputPipe,print_fmt,fmt);
    }

    template<typename... Args>
    STATICINLINE void cBasicPrint(cstring f, Args... a)
    {
        if(PrintingVerbosityLevel < 1)
            return;

        CString fmt = cStringFormat(f,a...);
        OutputPrinter::fprintf(DefaultDebugOutputPipe,print_fmt,fmt);
    }
    template<typename... Args>
    STATICINLINE void cBasicPrintNoNL(cstring f, Args... a)
    {
        if(PrintingVerbosityLevel < 1)
            return;

        OutputPrinter::fprintf(DefaultDebugOutputPipe,f,a...);
    }

    template<typename... Args>
    STATICINLINE void cVerbose(uint32 level, cstring f, Args... a)
    {
#ifndef NDEBUG
        if(PrintingVerbosityLevel < level)
            return;

        CString fmt = FormatPrintString(Severity::Verbose,
                                        0,f,a...);
        OutputPrinter::fprintf(DefaultDebugOutputPipe,print_fmt,fmt);
#endif
    }
};

using DebugPrinter = DebugPrinterImpl;

}
}

#endif
