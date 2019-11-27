#include <coffee/core/CProfiling>
#include <coffee/core/base.h>
#include <coffee/core/coffee.h>
#include <peripherals/stl/string_ops.h>

#include <coffee/strings/libc_types.h>

#include <coffee/core/CDebug>

#if defined(COFFEE_WINDOWS) && !defined(__MINGW64__)
#if !defined(COFFEE_WINDOWS_UWP)
extern bool WMI_Query(
    const char* query, const wchar_t* property, std::string& target);
#endif
extern int InitCOMInterface();
#endif

#if defined(COFFEE_GEKKO)
#include <coffee/gexxo/gexxo_api.h>
#endif

using namespace Coffee;

int deref_main(
    CoffeeMainWithArgs mainfun, int argc, char** argv, Coffee::u32 flags = 0)
{
#if !defined(COFFEE_LOWFAT) && 0
    cDebug("Entering deref_main() at {0}", str::print::pointerify(deref_main));
#endif

#if defined(COFFEE_WINDOWS) && !defined(COFFEE_WINDOWS_UWP) && \
    !defined(__MINGW64__)
#if MODE_RELEASE
    ShowWindow(GetConsoleWindow(), SW_HIDE);
#else
    if(Env::GetVar("VisualStudioVersion").size())
        ShowWindow(GetConsoleWindow(), SW_HIDE);
#endif
    InitCOMInterface();
#elif defined(COFFEE_WINDOWS_UWP)
    InitCOMInterface();
#elif defined(COFFEE_GEKKO)
    gexxo::initialize();
    printf("- Gamecube video initialized\n");
#endif

    int stat = Coffee::CoffeeMain(mainfun, argc, argv, flags);

#if defined(COFFEE_GEKKO)
    gexxo::infiniteLoop();
#endif

#ifndef COFFEE_CUSTOM_EXIT_HANDLING
#if MODE_DEBUG
    platform::profiling::PContext::ProfilerStore()->disable();
#endif
    exit(stat);
#else
    return stat;
#endif
}

#if !defined(COFFEE_PLAIN_INT_TYPES)
extern "C" int deref_main_c(int (*mainfun)(int, char**), int argc, char** argv)
{
    return deref_main(mainfun, argc, argv);
}
#endif

#include <peripherals/build/application.h>

namespace Coffee {

const platform::info::AppData& ApplicationData()
{
    return GetCurrentApp();
}
} // namespace Coffee
