#pragma once

#include <coffee/core/libc_types.h>
#include <coffee/core/types/application_main.h>
#include <peripherals/build/application.h>
#include <peripherals/identify/compiler/function_export.h>
#include <peripherals/stl/types.h>

namespace Coffee {

/*!
 * \brief This function handles initial program startup
 */
COFFEE_APP_FUNC i32
CoffeeMain(MainWithArgs mainfun, i32 argc, cstring_w* argv, u32 flags = 0);

/*!
 * \brief Initializes process state before running. Mainly core dumping.
 */
COFFEE_APP_FUNC void CoffeeInit(bool profiler_init = true);

COFFEE_APP_FUNC void CoffeeTerminate();

COFFEE_APP_FUNC void GotoApplicationDir();

COFFEE_APP_FUNC void InstallDefaultSigHandlers();

COFFEE_APP_FUNC void InstallStacktraceWriter();

COFFEE_APP_FUNC void SetPrintingVerbosity(u8 level);

extern void SetCurrentApp(platform::info::AppData const& app);

extern platform::info::AppData const& GetCurrentApp();

C_DEPRECATED_S("use GetCurrentApp()")

FORCEDINLINE platform::info::AppData const& ApplicationData()
{
    return GetCurrentApp();
}

} // namespace Coffee
