#include <coffee/core/plat/memory/cmd_interface.h>

#include <coffee/core/CDebug>

namespace Coffee{
namespace CmdInterface{

void BasicTerm::Exit(int code)
{
    cBasicPrint("Exiting with code {0}", code);

#if defined(COFFEE_EMSCRIPTEN)
    emscripten_force_exit(code);
#else
    exit(code);
#endif
}

#if defined(COFFEE_ANDROID)
static Vector<void(*)()> exit_functions;
#endif

void BasicTerm::RegisterAtExit(void (*efun)())
{
#if defined(COFFEE_ANDROID)
    cDebug("Registering function: {0}", efun);
    exit_functions.push_back(efun);
#else
    if(atexit(efun) != 0)
        fprintf(DefaultDebugOutputPipe,
                "Failed to register atexit() function\n");
#endif
}

#if defined(COFFEE_ANDROID)
const Vector<void (*)()> &BasicTerm::GetAtExit()
{
    return exit_functions;
}
#endif

}
}
