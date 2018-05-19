#include <coffee/windowing/windowing/ogc/ogc_window.h>

#if defined(COFFEE_GEKKO)

#include <gccore.h>
#include <malloc.h>
#include <ogcsys.h>

vu8 readyForCopy;
extern GXRModeObj* gamecube_rmode = NULL;
extern void* gamecube_xfb = NULL;

static void GC_CopyBuffers(::u32 count __attribute__((unused)))
{
    if(readyForCopy == GX_TRUE)
    {
        GX_SetZMode(GX_TRUE, GX_LEQUAL, GX_TRUE);
        GX_SetColorUpdate(GX_TRUE);
        GX_CopyDisp(gamecube_xfb, GX_TRUE);
        GX_Flush();
        readyForCopy = GX_FALSE;
    }
}

namespace Coffee{
namespace Display{

CString OGC_Window::windowLibrary() const
{
    return "Gamecube/VI";
}

CSize OGC_Window::framebufferSize() const
{
    return {gamecube_rmode->fbWidth, gamecube_rmode->xfbHeight};
}

bool OGC_Window::windowPreInit(const CDProperties&, CString*)
{
    fifoBuffer = MEM_K0_TO_K1(memalign(32, 256 * 1024));
    memset(fifoBuffer, 0, 256 * 1024);

    VIDEO_SetPostRetraceCallback(GC_CopyBuffers);

    GX_Init(fifoBuffer, 256 * 1024);
    GX_SetViewport(0, 0,
                   gamecube_rmode->fbWidth,
                   gamecube_rmode->efbHeight,
                   0, 1);

    GX_SetCullMode(GX_CULL_BACK);

//    printf("EFB height: %u", gamecube_rmode->efbHeight);
//    printf("XFB height: %u", gamecube_rmode->xfbHeight);

//    printf("FB width: %u", gamecube_rmode->fbWidth);

//    printf("TV mode: %u", gamecube_rmode->viTVMode);

//    printf("VI size: %ux%u+%ux%u",
//           gamecube_rmode->viXOrigin, gamecube_rmode->viYOrigin,
//           gamecube_rmode->viWidth, gamecube_rmode->viHeight);

    return true;
}

void OGC_Window::swapBuffers()
{
//    if(readyForCopy == GX_TRUE)
//    {
//        GX_AbortFrame();
//        return;
//    }

    readyForCopy = GX_TRUE;
    GX_DrawDone();

    VIDEO_WaitVSync();
}

}
}

#endif
