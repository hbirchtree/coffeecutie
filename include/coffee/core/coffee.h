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
extern int32 CoffeeMain(
        CoffeeMainWithArgs mainfun,
        int32 argc, cstring_w *argv,
        u32 flags = 0);

/*!
 * \brief Initializes process state before running. Mainly core dumping.
 */
extern void CoffeeInit(bool profiler_init = true);

extern void CoffeeTerminate();

extern void GotoApplicationDir();

extern void InstallDefaultSigHandlers();

extern void SetPrintingVerbosity(u8 level);

extern void SetDefaultWindowName(cstring appname);

extern CoffeeApplicationData const& ApplicationData();

//A simpler implementation of QObject
class CObject;

namespace CResources {

extern void FileResourcePrefix(cstring prefix);

extern CString const& GetFileResourcePrefix();

}

namespace CFunctional{}

using namespace CFunctional;

} //Coffee

#endif // COFFEE_H
