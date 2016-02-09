#include <coffee/core/base/cdebug.h>

#include <coffee/core/plat/plat_core.h>

namespace Coffee{
namespace DebugFun{

/* Defining storage for static variables */

Mutex OutputPrinterDef::PrinterLock;
Mutex DebuggingState::PrinterLock;

}

/* The below functions are exported to SWIG */

void cDebug(cstring f)
{
    DebugPrinter::cDebug(f);
}
void cWarning(cstring f)
{
    DebugPrinter::cWarning(f);
}
void cFatal(cstring f)
{
    DebugPrinter::cFatal(f);
}
void cLog(cstring id, cstring msg)
{
    cBasicPrint("LOGR_EXTERNAL:{0}: {1}",id,msg);
}

}
