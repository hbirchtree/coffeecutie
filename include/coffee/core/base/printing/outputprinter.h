#ifndef COFFEE_CORE_BASE_DEBUG_OUTPUT_PRINTER_H
#define COFFEE_CORE_BASE_DEBUG_OUTPUT_PRINTER_H

#include "../strings/cdebug_print.h"
#include "../debug/debug_interface.h"

#if defined(COFFEE_ANDROID)
#include <android/log.h>
#elif defined(__EMSCRIPTEN__)
#include <emscripten.h>
#endif

namespace Coffee{
namespace DebugFun{

struct OutputPrinterImpl : OutputPrinterDef
{
    static void fprintf_platform(FILE* stream, CString formatted,
                                 Severity sev, bool locking);

    template<typename... Args>
    static void fprintf(FILE* stream, Severity sev, cstring format,
                        Args... args)
    {
        C_UNUSED(sev);

        bool locking = false;

        CString formatted = Strings::cStringFormat(format,args...);

        /* If printing to file, don't lock IO */
        if(stream==stdout||stream==stderr)
        {
            locking = true;
            formatted = StrUtil::printclean(formatted);
        }

        fprintf_platform(stream, &formatted[0], sev, locking);
    }
};

using OutputPrinter = OutputPrinterImpl;

}
}

#endif
