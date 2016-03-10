#ifndef COFFEE_H
#define COFFEE_H

#include "exit_handling.h"
#include "argument_handling.h"
#include "types/tdef/fntypes.h"
#include "coffee_macros.h"

#include "platform_data.h"

namespace Coffee{

const constexpr cstring CoffeeBuildString = C_CONSTRUCT_BUILD_STRING("01.00",__CBDATETIME__);
const constexpr cstring CoffeeCompilerString = C_COMPILER_DEFINE_STRING(C_COMPILER_NAME,
                                                                  C_COMPILER_VER_MAJ,
                                                                  C_COMPILER_VER_MIN,
                                                                  C_COMPILER_VER_REV);

const constexpr cstring CoffeeArchString = COFFEE_ARCH;

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
