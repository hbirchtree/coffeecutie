#include <coffee/core/base/debug/debugprinter.h>

namespace Coffee{

uint8 PrintingVerbosityLevel = 0;

/* Move exported functions to common place, not spreading it like this */

using namespace DebugFun;

/* The below functions are exported to SWIG */

void cDebug(cstring f)
{
    DebugPrinter::cDebug("{0}",f);
}
void cWarning(cstring f)
{
    DebugPrinter::cWarning("{0}",f);
}
void cFatal(cstring f)
{
    DebugPrinter::cFatal("{0}",f);
}
void cLog(cstring id, cstring msg)
{
    /* TODO: Pipe this to a proper logger */
    DebugPrinter::cBasicPrint("LOGR_EXTERNAL:{0}: {1}",id,msg);
}

}
