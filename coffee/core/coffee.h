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

constexpr cstring cbuild_string = C_CONSTRUCT_BUILD_STRING("01.00",__CBDATETIME__);

inline void print_builddate()
{
    cDebug("Build date: {0}",cbuild_string);
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
