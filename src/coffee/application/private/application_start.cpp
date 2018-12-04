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
#include <gccore.h>

GXRModeObj* gamecube_rmode = NULL;
void*       gamecube_xfb   = NULL;

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
    if(gamecube_rmode->viTVMode & VI_NON_INTERLACE)
        VIDEO_WaitVSync();

    console_init(
        gamecube_xfb,
        60,
        60,
        gamecube_rmode->fbWidth,
        gamecube_rmode->xfbHeight,
        gamecube_rmode->fbWidth * 2);

    printf("- Gamecube video initialized\n");
}

void GCInfiniteLoop()
{
    while(1)
    {
        VIDEO_WaitVSync();
    }
}
#endif

using namespace Coffee;

int deref_main(
    CoffeeMainWithArgs mainfun, int argc, char** argv, Coffee::u32 flags = 0)
{
#ifndef COFFEE_LOWFAT
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
    GCVideoInit();
#endif

    int stat = Coffee::CoffeeMain(mainfun, argc, argv, flags);

#if defined(COFFEE_GEKKO)
    GCInfiniteLoop();
#endif

#ifndef COFFEE_CUSTOM_EXIT_HANDLING
#if MODE_DEBUG
    State::GetProfilerStore()->disable();
#endif
    exit(stat);
#else
    return stat;
#endif
}

extern "C" int deref_main_c(int (*mainfun)(int, char**), int argc, char** argv)
{
    return deref_main(mainfun, argc, argv);
}

#include <peripherals/build/application.h>

namespace Coffee {

const platform::info::AppData& ApplicationData()
{
    return GetCurrentApp();
}
} // namespace Coffee