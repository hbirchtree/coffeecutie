#ifndef COFFEE_H
#define COFFEE_H

#include "coffee_signals.h"
#include "argument_handling.h"
#include "types/tdef/fntypes.h"
#include "coffee_version.h"

#include "platform_data.h"
#include "eventprocess.h"

namespace Coffee{

/*!
 * \brief This function handles initial program startup
 */
extern int32 CoffeeMain(CoffeeMainWithArgs mainfun, int32 argc, cstring_w *argv);

/*!
 * \brief Initializes process state before running. Mainly core dumping.
 */
extern void CoffeeInit(bool profiler_init = true);

extern void CoffeeTerminate(bool profiler_destroy = true);

extern void GotoApplicationDir();

extern void InstallDefaultSigHandlers();

extern void SetPrintingVerbosity(u8 level);

extern CoffeeApplicationData const& ApplicationData();

//A simpler implementation of QObject
class CObject;

namespace CFunctional{}

using namespace CFunctional;

} //Coffee

#endif // COFFEE_H
