#include <coffee/core/base/debug/debugprinter.h>

namespace Coffee{

using namespace DebugFun;

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
    DebugPrinter::cBasicPrint("LOGR_EXTERNAL:{0}: {1}",id,msg);
}

}
