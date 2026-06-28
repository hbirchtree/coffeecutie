#include <ogc/video.h>
#include <peripherals/identify/system.h>

#include <debug.h>
#include <fat.h>
#include <gccore.h>
#include <platforms/gekko/dvd.h>
#include <sdcard/gcsd.h>
#include <stdio.h>

namespace gekko::console {
void init()
{
    static void*       xfb   = nullptr;
    static GXRModeObj* rmode = nullptr;
    
    printf("Booting console\n");

    VIDEO_Init();
    rmode = VIDEO_GetPreferredMode(nullptr);
    xfb   = MEM_K0_TO_K1(SYS_AllocateFramebuffer(rmode));

    console_init(
        xfb, 20, 20, rmode->fbWidth, rmode->xfbHeight,
        rmode->fbWidth * VI_DISPLAY_PIX_SZ);

    VIDEO_Configure(rmode);
    VIDEO_SetNextFramebuffer(xfb);
    VIDEO_SetBlack(FALSE);
    VIDEO_Flush();
    VIDEO_WaitVSync();
    if(rmode->viTVMode & VI_NON_INTERLACE)
        VIDEO_WaitVSync();

    // Unbuffered stdout, otherwise output is lost if the program aborts before
    // the buffer flushes (manifests as a blank screen).
    setvbuf(stdout, nullptr, _IONBF, 0);
    setvbuf(stderr, nullptr, _IONBF, 0);
}

void infinite_loop()
{
    while(true)
        VIDEO_WaitVSync();
}
}

namespace gekko {
void mount_debugger()
{
    DEBUG_Init(GDBSTUB_DEVICE_USB, 0);
    printf("Waiting for debugger to attach...\n");
    _break();
}

void mount_storage()
{
    setvbuf(stdout, nullptr, _IONBF, 0); // flush each line (debug visibility)
    if(platform::file::gekko::dvd::mount())
    {
        printf("- DVD mounted, assets at dvd:/\n");
    } else
        printf("- DVD mount failed (no disc?)\n");

    if(fatMountSimple("sd", &__io_gcsda))
        printf("- SD mounted read/write at sd:/\n");
    else
        printf("- SD not present (sd:/ unavailable)\n");
}
}
