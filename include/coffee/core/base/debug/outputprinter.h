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
    static void fprintf(FILE* stream, Severity sev, cstring format, Args... args)
    {
        C_UNUSED(sev);

        bool locking = false;

        CString formatted = cStringFormat(format,args...);

        /* If printing to file, don't lock IO */
        if(stream==stdout||stream==stderr)
        {
            locking = true;
            formatted = StrUtil::printclean(formatted);
        }

        if(locking)
            PrinterLock.lock();
#if defined(COFFEE_ANDROID)
        int flag = 0;

        switch(sev)
        {
        case Severity::Information:
            flag = ANDROID_LOG_INFO;
            break;
        case Severity::Debug:
            flag = ANDROID_LOG_DEBUG;
            break;
        case Severity::Medium:
            flag = ANDROID_LOG_WARN;
            break;
        case Severity::Fatal:
            flag = ANDROID_LOG_ERROR;
            break;
        case Severity::Verbose:
            flag = ANDROID_LOG_VERBOSE;
            break;
	default:
	    break;
        }

        __android_log_print(flag, "Coffee", "%s", &formatted[0]);
#elif defined(__EMSCRIPTEN__)
        int flag = EM_LOG_CONSOLE;
        if(formatted[0] == 'W')
            flag = EM_LOG_WARN;
        else if(formatted[0] == 'F')
            flag = EM_LOG_ERROR;
        emscripten_log(flag, "%s", &formatted[0]);
#elif defined(COFFEE_WINDOWS_UWP)
		CWString formatted_w = StrUtil::convertformat<wbyte_t>(formatted);
		OutputDebugString(formatted_w.c_str());
#elif defined(COFFEE_WINDOWS)
		OutputDebugString(formatted.c_str());
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
