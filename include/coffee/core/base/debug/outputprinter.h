#ifndef COFFEE_CORE_BASE_DEBUG_OUTPUT_PRINTER_H
#define COFFEE_CORE_BASE_DEBUG_OUTPUT_PRINTER_H

#include "cdebug_print.h"
#include "debug_interface.h"

#ifdef COFFEE_ANDROID
#include <android/log.h>
#endif

namespace Coffee{
namespace DebugFun{

struct OutputPrinterImpl : OutputPrinterDef
{
    template<typename... Args>
    static void fprintf(FILE* stream, cstring format, Args... args)
    {
        bool locking = false;

        /* If printing to file, don't lock IO */
        if(stream==stdout||stream==stderr)
            locking = true;

        CString formatted = cStringFormat(format,args...);
        if(locking)
            PrinterLock.lock();
#if defined(COFFEE_ANDROID)
        int flag = ANDROID_LOG_DEBUG;
        if(formatted.substr(0,4) == "WARN")
            flag = ANDROID_LOG_WARN;
        else if(formatted.substr(0,4) == "FTAL")
            flag = ANDROID_LOG_ERROR;
        __android_log_print(flag, "Coffee", formatted.c_str());
#else
        Puts(stream,formatted.c_str());
#endif
        if(locking)
            PrinterLock.unlock();
    }
};

using OutputPrinter = OutputPrinterImpl;

}
}

#endif
