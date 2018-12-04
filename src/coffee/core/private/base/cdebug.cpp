#include <coffee/strings/libc_types.h>

#include <coffee/core/terminal/cursor.h>
#include <coffee/strings/format.h>

#include <coffee/core/CDebug>

namespace Coffee {

/* Move exported functions to common place, not spreading it like this */

/* The below functions are exported to SWIG */

void cDebug(cstring f)
{
#ifndef COFFEE_LOWFAT
    cDebug("{0}", f);
#endif
}
void cWarning(cstring f)
{
#ifndef COFFEE_LOWFAT
    cWarning("{0}", f);
#endif
}
void cFatal(cstring f)
{
#ifndef COFFEE_LOWFAT
    cFatal("{0}", f);
#endif
}
void cLog(cstring id, cstring msg)
{
#ifndef COFFEE_LOWFAT
    /* TODO: Pipe this to a proper logger */
    cBasicPrint("LOGR_EXTERNAL:{0}: {1}", id, msg);
#endif
}

void TerminalCursor::print_basic(CString const& fmt, CString const& output)
{
#ifndef COFFEE_LOWFAT
    cBasicPrint(fmt.c_str(), output);
#endif
}

void TerminalCursor::print_nonl(CString const& fmt, CString const& output)
{
#ifndef COFFEE_LOWFAT
    cBasicPrintNoNL(fmt.c_str(), output);
#endif
}

} // namespace Coffee
