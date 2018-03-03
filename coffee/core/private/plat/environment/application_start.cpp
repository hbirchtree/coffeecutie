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

#if defined(COFFEE_GEKKO)
GXRModeObj* gamecube_rmode = NULL;
void* gamecube_xfb = NULL;

void GCVideoInit()
{
    VIDEO_Init();

    gamecube_rmode = VIDEO_GetPreferredMode(NULL);

    gamecube_xfb = MEM_K0_TO_K1(SYS_AllocateFramebuffer(gamecube_rmode));

    VIDEO_Configure(gamecube_rmode);
    VIDEO_SetNextFramebuffer(gamecube_xfb);
    VIDEO_SetBlack(FALSE);
    VIDEO_Flush();

    VIDEO_WaitVSync();
    if(gamecube_rmode->viTVMode&VI_NON_INTERLACE)
        VIDEO_WaitVSync();

    console_init(gamecube_xfb, 20, 20,
                 gamecube_rmode->fbWidth, gamecube_rmode->xfbHeight,
                 gamecube_rmode->fbWidth*2);
}

void GCInfiniteLoop()
{
    while(1) { VIDEO_WaitVSync(); }
}
#endif

using namespace Coffee;

int deref_main(CoffeeMainWithArgs mainfun,
               int argc, char** argv,
               u32 flags = 0)
{
    cDebug("Entering deref_main() at {0}", StrUtil::pointerify(deref_main));

#if defined(COFFEE_GEKKO)
    GCVideoInit();
#endif

#if defined(COFFEE_WINDOWS) && !defined(COFFEE_WINDOWS_UWP) \
    && !defined(__MINGW64__)
#ifdef NDEBUG
	ShowWindow(GetConsoleWindow(), SW_HIDE);
#else
	if(Env::GetVar("VisualStudioVersion").size())
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
    int stat = Coffee::CoffeeMain(mainfun,argc,argv, flags);

#if defined(COFFEE_GEKKO)
    GCInfiniteLoop();
#endif

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

#include <coffee/core/coffee_version.h>

namespace Coffee{
#ifndef COFFEE_LOADABLE_LIBRARY
extern CoffeeApplicationData app_data;
#endif

const CoffeeApplicationData& ApplicationData()
{
#ifdef COFFEE_LOADABLE_LIBRARY
    static CoffeeApplicationData app_data;
#endif
    return app_data;
}
}

