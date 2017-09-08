#include "../../coffee_mem_macros.h"

#include "../../types/cdef/geometry.h"
#include "cmemory.h"

#ifndef COFFEE_CORE_PLAT_COMMAND_INTERFACE_H
#define COFFEE_CORE_PLAT_COMMAND_INTERFACE_H

#if defined(COFFEE_USE_TERMINAL_CTL)
#include <sys/ioctl.h>
#include <unistd.h>
#endif

#ifdef COFFEE_ANDROID
#include <android/log.h>
#endif

#ifdef COFFEE_EMSCRIPTEN
#include <emscripten.h>
#endif

namespace Coffee{
namespace CmdInterface{

struct CmdDef
{
    /* Indicate whether we are using a proper terminal */
    static bool Interactive();

    /* Basic command line actions */
    static cstring ReadString(cstring_w target, int32 size, FILE* strm);
    static void Wait();
    static void Exit(int code);

    static void Execute(cstring cmd);
    static void Execute(cstring program, uint32 num_args, cstring* args);

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

    C_NORETURN STATICINLINE void Exit(int code)
    {
        const constexpr cstring printf_fmt = "Exiting with code: %i\n";
#if defined(COFFEE_ANDROID)
        __android_log_print(ANDROID_LOG_WARN, "Coffee", printf_fmt, code);
#else
        fprintf(DefaultPrintOutputPipe, printf_fmt, code);
#endif
#if defined(COFFEE_EMSCRIPTEN)
        emscripten_force_exit(code);
#else
        exit(code);
#endif
    }

    STATICINLINE void RegisterAtExit(void(*efun)())
    {
        if(atexit(efun) != 0)
            fprintf(DefaultDebugOutputPipe,
                    "Failed to register atexit() function\n");
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
