#ifndef COFFEE_H
#define COFFEE_H

#include <signal.h>

#include "types/basetypes.h"
#include "coffee_macros.h"

#include "plat/platform_detect.h"

namespace Coffee{

using ExitCallback = void(*)();

enum CoffeeExitCode
{
    CoffeeExit_Normal       = SIGQUIT,
    CoffeeExit_GeneralError = 1,

    CoffeeExit_UnknownBad  = 100,

    CoffeeExit_IllegalOp   = SIGILL,
    CoffeeExit_Termination = SIGTERM,
    CoffeeExit_Interrupt   = SIGINT,

    CoffeeExit_Kill        = SIGKILL,

    CoffeeExit_Pooped       = SIGSEGV,
    CoffeeExit_PoopedABit   = SIGABRT,
};

const constexpr cstring CoffeeBuildString = C_CONSTRUCT_BUILD_STRING("01.00",__CBDATETIME__);
const constexpr cstring CoffeeCompilerString = C_COMPILER_DEFINE_STRING(C_COMPILER_NAME,
                                                                  C_COMPILER_VER_MAJ,
                                                                  C_COMPILER_VER_MIN,
                                                                  C_COMPILER_VER_REV);
const constexpr cstring CoffeeArchString = COFFEE_ARCH;

extern void SetExitFunction(ExitCallback f);

/*!
 * \brief This function handles initial program startup
 */
extern int32 CoffeeMain(CoffeeMainWithArgs mainfun, int32 argc, cstring_w *argv);

/*!
 * \brief Initializes process state before running. Mainly core dumping.
 */
extern void CoffeeInit();

extern void CoffeeTerminate();

//A simpler implementation of QObject
class CObject;

namespace CFunctional{}

using namespace CFunctional;

} //Coffee

#endif // COFFEE_H
