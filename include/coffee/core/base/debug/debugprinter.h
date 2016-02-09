#ifndef COFFEE_CORE_BASE_DEBUG_DEBUGPRINTER_H
#define COFFEE_CORE_BASE_DEBUG_DEBUGPRINTER_H

#include "debug_interface.h"

#include "outputprinter.h"

namespace Coffee{
namespace DebugFun{

struct DebugPrinterImpl : DebugPrinterDef
{
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

        CString clock = Time::ClockString();
        CString prefix = cStringFormat("{0}:{1}:{2}",
                                       clock.c_str(),
                                       severity_str,
                                       "funnystuff(void)");
        CString formatted = cStringFormat(fmt,args...);

        return cStringFormat("{0}: {1}",prefix,formatted);
    }

    template<typename... Args>
    STATICINLINE void cDebug(cstring f, Args... a)
    {
        CString fmt = FormatPrintString(Severity::Debug,
                                        0,f,a...);
        OutputPrinter::fprintf(stderr,"{0}\n",fmt);
    }
    template<typename... Args>
    STATICINLINE void cWarning(cstring f, Args... a)
    {
        CString fmt = FormatPrintString(Severity::Medium,
                                        0,f,a...);
        OutputPrinter::fprintf(stderr,"{0}\n",fmt);
    }
    template<typename... Args>
    STATICINLINE void cFatal(cstring f, Args... a)
    {
        CString fmt = FormatPrintString(Severity::Fatal,
                                        0,f,a...);
        OutputPrinter::fprintf(stderr,"{0}\n",fmt);
    }

    template<typename... Args>
    STATICINLINE void cBasicPrint(cstring f, Args... a)
    {
        CString fmt = cStringFormat(f,a...);
        OutputPrinter::fprintf(stderr,"{0}\n",fmt);
    }
    template<typename... Args>
    STATICINLINE void cBasicPrintNoNL(cstring f, Args... a)
    {
        OutputPrinter::fprintf(stderr,f,a...);
    }
};

using DebugPrinter = DebugPrinterImpl;

}
}

#endif