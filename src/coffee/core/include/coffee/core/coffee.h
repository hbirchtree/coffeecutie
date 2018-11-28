#pragma once

#include <coffee/core/libc_types.h>
#include <coffee/core/stl_types.h>
#include <coffee/core/types/application_main.h>
#include <peripherals/build/application.h>

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

COFFEE_APP_FUNC platform::info::AppData const& ApplicationData();

extern void SetCurrentApp(platform::info::AppData const& app);

extern platform::info::AppData const& GetCurrentApp();

} // namespace Coffee
