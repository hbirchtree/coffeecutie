//#include <coffee/core/CApplication>
#include <coffee/core/coffee.h>
#include <coffee/core/CProfiling>
#include <coffee/core/CDebug>

#if defined(COFFEE_WINDOWS)
#include <coffee/core/plat/plat_windows.h>
#elif defined(COFFEE_ANDROID)
#include "../sensor/android/android_sensors.h"
#endif

using namespace Coffee;

int deref_main(CoffeeMainWithArgs mainfun, int argc, char** argv)
{
    cDebug("Entering deref_main() at {0}",StrUtil::pointerify((uint64)deref_main));
#if defined(COFFEE_WINDOWS) && !defined(COFFEE_WINDOWS_UWP)
	ShowWindow(GetConsoleWindow(), SW_HIDE);
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
