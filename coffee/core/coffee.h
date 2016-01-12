#ifndef COFFEE_H
#define COFFEE_H

#include "types/types.h"
#include "coffee_macros.h"

#include "plat/platform_detect.h"
#include "plat/cmemory.h"
#include "base/cdebug.h"

#include <thread>
#include <functional>

namespace Coffee{

enum CoffeeExitCode
{
    CoffeeExit_UnknownBad  = 100,

    CoffeeExit_Termination = 120,
    CoffeeExit_Interrupt   = 121,

    CoffeeExit_Kill        = 122,
};

const constexpr cstring CoffeeBuildString = C_CONSTRUCT_BUILD_STRING("01.00",__CBDATETIME__);
const constexpr cstring CoffeeCompilerString = C_COMPILER_DEFINE_STRING(C_COMPILER_NAME,
                                                                  C_COMPILER_VER_MAJ,
                                                                  C_COMPILER_VER_MIN,
                                                                  C_COMPILER_VER_REV);
const constexpr cstring CoffeeArchString = COFFEE_ARCH;

inline C_FORCE_INLINE void print_builddate()
{
}

/*!
 * \brief This function handles initial program startup
 */
extern int32 CoffeeMain(CoffeeMainWithArgs mainfun, int32 argv, byte_t** argc);

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
