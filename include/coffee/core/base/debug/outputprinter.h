#ifndef COFFEE_CORE_BASE_DEBUG_OUTPUT_PRINTER_H
#define COFFEE_CORE_BASE_DEBUG_OUTPUT_PRINTER_H

#include "cdebug_print.h"
#include "debug_interface.h"

#if defined(COFFEE_ANDROID)
#include <android/log.h>
#elif defined(__EMSCRIPTEN__)
#include <emscripten.h>
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
        int c_str_offset = 2;
        int flag = 0;

        if(formatted[0] == 'I')
            flag = ANDROID_LOG_INFO;
        else if(formatted[0] == 'D')
            flag = ANDROID_LOG_DEBUG;
        else if(formatted[0] == 'W')
            flag = ANDROID_LOG_WARN;
        else if(formatted[0] == 'F')
            flag = ANDROID_LOG_ERROR;
        else if(formatted[0] == 'V')
            flag = ANDROID_LOG_VERBOSE;

        __android_log_print(flag, "Coffee", "%s", &formatted[c_str_offset]);
#elif defined(__EMSCRIPTEN__)
        int flag = EM_LOG_CONSOLE;
        if(formatted[0] == 'W')
            flag = EM_LOG_WARN;
        else if(formatted[0] == 'F')
            flag = EM_LOG_ERROR;
        emscripten_log(flag, "%s", &formatted[2]);
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
