#ifndef COFFEE_H
#define COFFEE_H

#include <signal.h>

#include "types/cdef/infotypes.h"
#include "types/tdef/fntypes.h"
#include "coffee_macros.h"

#include "platform_data.h"

namespace Coffee{

using ExitCallback = void(*)();

enum CoffeeExitCode
{
#ifndef COFFEE_WINDOWS
    CoffeeExit_Normal       = SIGQUIT,
#else
    CoffeeExit_Normal       = 0,
#endif
    CoffeeExit_GeneralError = 1,

    CoffeeExit_UnknownBad  = 100,

    CoffeeExit_IllegalOp   = SIGILL,
    CoffeeExit_Termination = SIGTERM,
    CoffeeExit_Interrupt   = SIGINT,

#ifndef COFFEE_WINDOWS
    CoffeeExit_Kill        = SIGKILL,
#endif

    CoffeeExit_Pooped       = SIGSEGV,
    CoffeeExit_PoopedABit   = SIGABRT,
};

const constexpr cstring CoffeeBuildString = C_CONSTRUCT_BUILD_STRING("01.00",__CBDATETIME__);
const constexpr cstring CoffeeCompilerString = C_COMPILER_DEFINE_STRING(C_COMPILER_NAME,
                                                                  C_COMPILER_VER_MAJ,
                                                                  C_COMPILER_VER_MIN,
                                                                  C_COMPILER_VER_REV);

const constexpr cstring CoffeeArchString = COFFEE_ARCH;

extern int32 Startup_argc;
extern cstring_w* Startup_argv;

extern void SetExitFunction(ExitCallback f);

extern AppArg const& GetInitArgs();

/*!
 * \brief This function handles initial program startup
 */
extern int32 CoffeeMain(CoffeeMainWithArgs mainfun, int32 argc, cstring_w *argv);

/*!
 * \brief Initializes process state before running. Mainly core dumping.
 */
extern void CoffeeInit(bool profiler_init = true);

extern void CoffeeTerminate(bool profiler_destroy = true);

//A simpler implementation of QObject
class CObject;

namespace CFunctional{}

using namespace CFunctional;

} //Coffee

#endif // COFFEE_H
