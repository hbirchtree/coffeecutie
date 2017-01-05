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
        int c_str_offset = 0;
        int flag = ANDROID_LOG_INFO;
        if(formatted[0] == 'I')
        {
            flag = ANDROID_LOG_VERBOSE;
            c_str_offset = 6;
        }
        else if(formatted[0] == 'D')
        {
            flag = ANDROID_LOG_DEBUG;
            c_str_offset = 6;
        }
        else if(formatted[0] == 'W')
        {
            flag = ANDROID_LOG_WARN;
            c_str_offset = 6;
        }
        else if(formatted[0] == 'F')
        {
            flag = ANDROID_LOG_ERROR;
            c_str_offset = 6;
        }else if(formatted[0] == 'V')
        {
            flag = ANDROID_LOG_VERBOSE;
            c_str_offset = 6;
        }
        __android_log_print(flag, "Coffee", &formatted[c_str_offset]);
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
