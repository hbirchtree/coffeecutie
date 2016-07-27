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
    STATICINLINE CString& Colorize(CString& s, Color back, Color front)
    {
        cstring bg = "";
        cstring fg = "";
        switch(back)
        {
        case Color::Yellow:
            bg = ColorMap::BgYellow;
            break;
        case Color::Blue:
            bg = ColorMap::BgBlue;
            break;
        case Color::Red:
            bg = ColorMap::BgRed;
            break;
        case Color::Green:
            bg = ColorMap::BgDarkGreen;
            break;
        default:
            break;
        }
        switch(front)
        {
        case Color::Yellow:
            fg = ColorMap::FontYellow;
            break;
        case Color::Red:
            fg = ColorMap::FontRed;
            break;
        case Color::Blue:
            fg = ColorMap::FontBlue;
            break;
        case Color::Green:
            fg = ColorMap::FontDarkGreen;
            break;
        default:
            break;
        }

        s = fg + s;
        s = bg + s;
        s.append(ColorMap::Reset);

        return s;
    }

    template<typename... Args>
    STATICINLINE CString FormatPrintString(Severity sev, uint32 stackoffset,
                                     cstring fmt, Args... args)
    {
        cstring const severity_info = "INFO";
        cstring const severity_debg = "DEBG";
        cstring const severity_warn = "WARN";
        cstring const severity_ftal = "FTAL";

        cstring severity_str = nullptr;

        switch(sev)
        {
        case Severity::Information:
            severity_str = severity_info;
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
#ifndef COFFEE_ANDROID
        CString clock = cStringFormat("{0}.{1}",cclock,Time::Microsecond()/1000);
        CString prefix = cStringFormat("{0}:{1}:{2}",
                                       clock.c_str(),
                                       severity_str,
                                       "???");
#else
        CString prefix = severity_str;
#endif

        Colorize(prefix,Color::Blue,Color::Green);

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
        OutputPrinter::fprintf(DefaultDebugOutputPipe,"{0}\n",fmt);
    }
    template<typename... Args>
    STATICINLINE void cWarning(cstring f, Args... a)
    {
        if(PrintingVerbosityLevel < 1)
            return;

        CString fmt = FormatPrintString(Severity::Medium,
                                        0,f,a...);
        OutputPrinter::fprintf(DefaultDebugOutputPipe,"{0}\n",fmt);
    }
    template<typename... Args>
    STATICINLINE void cFatal(cstring f, Args... a)
    {
        CString fmt = FormatPrintString(Severity::Fatal,
                                        0,f,a...);
        OutputPrinter::fprintf(DefaultDebugOutputPipe,"{0}\n",fmt);
    }

    template<typename... Args>
    STATICINLINE void cBasicPrint(cstring f, Args... a)
    {
        if(PrintingVerbosityLevel < 1)
            return;

        CString fmt = cStringFormat(f,a...);
        OutputPrinter::fprintf(DefaultDebugOutputPipe,"{0}\n",fmt);
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

        CString fmt = FormatPrintString(Severity::Information,
                                        0,f,a...);
        OutputPrinter::fprintf(DefaultDebugOutputPipe,"{0}\n",fmt);
#endif
    }
};

using DebugPrinter = DebugPrinterImpl;

}
}

#endif
