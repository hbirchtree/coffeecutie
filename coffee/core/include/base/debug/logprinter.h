#ifndef COFFEE_CORE_BASE_DEBUG_LOGPRINTER_H
#define COFFEE_CORE_BASE_DEBUG_LOGPRINTER_H

#include "debug_interface.h"

#include "debugprinter.h"

namespace Coffee{
namespace DebugFun{

struct LogPrinterImpl : LogPrinterDef
{
    template<typename... Args>
    static void cLog(cstring file, int64 line, cstring component, cstring fmt, Args... args)
    {
#ifndef NDEBUG
        CString msg_out = cStringFormat(fmt,args...);
        DebugPrinter::cBasicPrint("LOGR:{0}@{3}:{1}: {2}",file,component,msg_out.c_str(),line);
#endif
    }
    template<typename... Args>
    static void cMsg(cstring src, cstring msg, Args... args)
    {
        CString msg_out = cStringFormat(msg,args...);
        DebugPrinter::cDebug(0,1,"{0}: {1}",src,msg_out.c_str());
    }
};

using LogPrinter = LogPrinterImpl;

}
}

#endif