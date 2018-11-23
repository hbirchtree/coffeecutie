#pragma once

#include "argument_handling.h"
#include "types/tdef/fntypes.h"

#include "platform_data.h"

namespace Coffee {

/*!
 * \brief This function handles initial program startup
 */
extern i32 CoffeeMain(
    CoffeeMainWithArgs mainfun, i32 argc, cstring_w* argv, u32 flags = 0);

/*!
 * \brief Initializes process state before running. Mainly core dumping.
 */
COFFEE_APP_FUNC void CoffeeInit(bool profiler_init = true);

COFFEE_APP_FUNC void CoffeeTerminate();

COFFEE_APP_FUNC void GotoApplicationDir();

COFFEE_APP_FUNC void InstallDefaultSigHandlers();

COFFEE_APP_FUNC void SetPrintingVerbosity(u8 level);

extern void SetDefaultWindowName(cstring appname);

COFFEE_APP_FUNC AppData const& ApplicationData();

extern void SetCurrentApp(AppData const& app);

extern AppData const& GetCurrentApp();

namespace CResources {

extern void FileResourcePrefix(cstring prefix);

extern CString const& GetFileResourcePrefix();

} // namespace CResources

} // namespace Coffee
