#include <coffee/core/plat/plat_primary_identify.h>

#if defined(COFFEE_WINDOWS)
#if !defined(COFFEE_WINDOWS_UWP)
extern bool WMI_Query(const char* query, const wchar_t* property);
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
    cDebug("Entering deref_main() at {0}",StrUtil::pointerify((uint64)deref_main));
#if defined(COFFEE_WINDOWS) && !defined(COFFEE_WINDOWS_UWP)
#ifdef NDEBUG
	ShowWindow(GetConsoleWindow(), SW_HIDE);
#endif
    InitCOMInterface();
#elif defined(COFFEE_WINDOWS_UWP)
	InitCOMInterface();
#elif defined(COFFEE_ANDROID)
    Profiler::ResetPointers();
    Sensor::Android::Android_InitSensors();
#endif
    int stat = Coffee::CoffeeMain(mainfun,argc,argv);
#if defined(COFFEE_ANDROID)
    Sensor::Android::Android_DestroySensors();
#endif
    return stat;
}
