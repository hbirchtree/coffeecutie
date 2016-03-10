#include "../../coffee_mem_macros.h"

#include "../../types/cdef/geometry.h"
#include "cmemory.h"

#ifndef COFFEE_CORE_PLAT_COMMAND_INTERFACE_H
#define COFFEE_CORE_PLAT_COMMAND_INTERFACE_H

#include "../environment/environment_details.h"

#if defined(COFFEE_USE_TERMINAL_CTL)
#include <sys/ioctl.h>
#include <unistd.h>
#endif

namespace Coffee{
namespace CmdInterface{

using TermScreen = Env::TermScreen;

struct CmdDef
{
    /* Indicate whether we are using a proper terminal */
    static bool Interactive();

    /* Basic command line actions */
    static cstring ReadString(cstring_w target, int32 size, FILE* strm);
    static void Wait();
    static void Exit(int code);

    /* Terminal screens */
    static void ClearScreen();
    static void AltScreen();
    static void ResetScreen();
    static CSize TerminalSize();
};

struct BasicTerm : CmdDef
{
    STATICINLINE cstring ReadString(cstring_w target, int32 size, FILE* strm)
    {
        return Gets(target,size,strm);
    }

    STATICINLINE void Wait()
    {
        getchar();
    }

    STATICINLINE void Exit(int code)
    {
        fprintf(DefaultDebugOutputPipe,
                "Exiting with code: %i\n",
                code);
        exit(code);
    }
};

}
}

#endif

#undef COFFEE_CMD_INTERFACE_STRUCT

#if defined(COFFEE_USE_TERMINAL_CTL)
#include "cmd_unixterm.h"
#else
#include "cmd_dummy.h"
#endif