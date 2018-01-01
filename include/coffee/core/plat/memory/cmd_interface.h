#pragma once
#include "../../coffee_mem_macros.h"

#include "../../types/cdef/geometry.h"
#include "cmemory.h"

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

    C_NORETURN static void Exit(int code);

    static void RegisterAtExit(void(*efun)());

#if defined(COFFEE_NO_ATEXIT)
    static Vector<void(*)()> const& GetAtExit();
#endif
};

}
}
