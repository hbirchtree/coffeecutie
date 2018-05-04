#include <coffee/core/base/debug/debugprinter.h>
#include <coffee/core/terminal/terminal_cursor.h>

namespace Coffee{

/* Move exported functions to common place, not spreading it like this */

using namespace DebugFun;

/* The below functions are exported to SWIG */

void cDebug(cstring f)
{
#ifndef COFFEE_LOWFAT
    DebugPrinter::cDebug("{0}",f);
#endif
}
void cWarning(cstring f)
{
#ifndef COFFEE_LOWFAT
    DebugPrinter::cWarning("{0}",f);
#endif
}
void cFatal(cstring f)
{
#ifndef COFFEE_LOWFAT
    DebugPrinter::cFatal("{0}",f);
#endif
}
void cLog(cstring id, cstring msg)
{
#ifndef COFFEE_LOWFAT
    /* TODO: Pipe this to a proper logger */
    DebugPrinter::cBasicPrint("LOGR_EXTERNAL:{0}: {1}",id,msg);
#endif
}

void TerminalCursor::print_basic(CString const& fmt, CString const& output)
{
#ifndef COFFEE_LOWFAT
    DebugPrinter::cBasicPrint(fmt.c_str(), output);
#endif
}

void TerminalCursor::print_nonl(CString const& fmt, CString const& output)
{
#ifndef COFFEE_LOWFAT
    DebugPrinter::cBasicPrintNoNL(fmt.c_str(), output);
#endif
}

}
