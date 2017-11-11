#include <coffee/core/plat/plat_primary_identify.h>

#include <string>

#if defined(COFFEE_WINDOWS) && !defined(__MINGW64__)
#if !defined(COFFEE_WINDOWS_UWP)
extern bool WMI_Query(const char* query, const wchar_t* property, std::string& target);
#endif
extern int InitCOMInterface();
#elif defined(COFFEE_ANDROID)
#include "../sensor/android/android_sensors.h"
#endif

#include <coffee/core/coffee.h>
#include <coffee/core/CProfiling>
#include <coffee/core/CDebug>

using namespace Coffee;

int deref_main(CoffeeMainWithArgs mainfun, int argc, char** argv)
{
    cDebug("Entering deref_main() at {0}", StrUtil::pointerify(deref_main));
#if defined(COFFEE_WINDOWS) && !defined(COFFEE_WINDOWS_UWP) \
    && !defined(__MINGW64__)
#ifdef NDEBUG
	ShowWindow(GetConsoleWindow(), SW_HIDE);
#endif
    InitCOMInterface();
#elif defined(COFFEE_WINDOWS_UWP)
	InitCOMInterface();
#elif defined(COFFEE_ANDROID)
    Profiler::ResetPointers();
    Sensor::Android::Android_InitSensors();

    atexit(Sensor::Android::Android_DestroySensors);
#endif
    int stat = Coffee::CoffeeMain(mainfun,argc,argv);

#ifndef COFFEE_CUSTOM_EXIT_HANDLING
    exit(stat);
#else
    return stat;
#endif
}

extern "C" int deref_main_c(int(*mainfun)(int, char**), int argc, char** argv)
{
    return deref_main(mainfun, argc, argv);
}
