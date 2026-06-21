#include <coffee/gexxo/gexxo_api.h>

#include <gccore.h>
#include <malloc.h>

#include <cstring>

/* GameCube VI + GX bring-up. This is the low-level video plumbing the RHI api
 * (gexxo::api::load), the application bootstrap (coffee.cpp) and the comp_app
 * windowing bridge (cog_comp) build on. Reused largely unchanged from the
 * previous Gexxo implementation. */

namespace gexxo {

namespace {

struct Context
{
    libc_types::u32      currentFb = 0;
    std::array<void*, 2> framebuffer{{nullptr, nullptr}};
    GXRModeObj*          rmode   = nullptr;
    void*               gxFifo  = nullptr;
    bool                gxReady = false;
};

Context* g_context = nullptr;

} // namespace

void initialize()
{
    if(g_context)
        return;

    g_context = new Context();

    VIDEO_Init();

    g_context->rmode = VIDEO_GetPreferredMode(nullptr);

    for(int i = 0; i < 2; i++)
        g_context->framebuffer[i] =
            MEM_K0_TO_K1(SYS_AllocateFramebuffer(g_context->rmode));

    VIDEO_Configure(g_context->rmode);
    VIDEO_SetNextFramebuffer(g_context->framebuffer[0]);
    VIDEO_SetBlack(FALSE);
    VIDEO_Flush();
    VIDEO_WaitVSync();

    if(g_context->rmode->viTVMode & VI_NON_INTERLACE)
        VIDEO_WaitVSync();

    console_init(
        g_context->framebuffer[0],
        20,
        20,
        g_context->rmode->fbWidth,
        g_context->rmode->xfbHeight,
        g_context->rmode->fbWidth * 2);

    /* Route stdout+stderr to the Dolphin OSReport UART and replace the VI
     * console devoptab. All output (printf, Coffee logging) now goes to
     * OSReport (visible in Dolphin's log) instead of rendering into the XFB,
     * which both provides logging and stops the on-screen console from
     * flashing over the GX framebuffer. No-ops on real hardware. */
    SYS_STDIO_Report(true);
}

void gxInitialize()
{
    static constexpr libc_types::u32 default_fifo_size = 256 * 1024;

    if(!g_context)
        initialize();
    if(g_context->gxReady)
        return;

    g_context->gxFifo = memalign(32, default_fifo_size);
    std::memset(g_context->gxFifo, 0, default_fifo_size);

    GX_Init(g_context->gxFifo, default_fifo_size);

    GXColor clearColor = {0x00, 0x00, 0x00, 0xff};
    GX_SetCopyClear(clearColor, 0x00ffffff);

    auto* rmode = g_context->rmode;

    GX_SetViewport(0, 0, rmode->fbWidth, rmode->efbHeight, 0.f, 1.f);
    auto yscale    = GX_GetYScaleFactor(rmode->efbHeight, rmode->xfbHeight);
    auto xfbHeight = GX_SetDispCopyYScale(yscale);
    GX_SetDispCopySrc(0, 0, rmode->fbWidth, rmode->efbHeight);
    GX_SetDispCopyDst(rmode->fbWidth, xfbHeight);
    GX_SetCopyFilter(
        rmode->aa, rmode->sample_pattern, GX_TRUE, rmode->vfilter);
    GX_SetFieldMode(
        rmode->field_rendering,
        (rmode->viHeight == rmode->xfbHeight * 2) ? GX_ENABLE : GX_DISABLE);
    GX_SetPixelFmt(GX_PF_RGB8_Z24, GX_ZC_LINEAR);
    GX_SetCullMode(GX_CULL_NONE);

    g_context->gxReady = true;
}

void infiniteLoop()
{
    while(true)
        VIDEO_WaitVSync();
}

void swapBuffers()
{
    if(!g_context)
        return;

    if(g_context->gxReady)
    {
        GX_DrawDone();
        GX_CopyDisp(g_context->framebuffer[g_context->currentFb], GX_TRUE);
    }

    VIDEO_SetNextFramebuffer(g_context->framebuffer[g_context->currentFb]);
    VIDEO_Flush();
    VIDEO_WaitVSync();

    (++g_context->currentFb) %= 2;
}

typing::geometry::size_2d<libc_types::i32> framebufferSize()
{
    if(!g_context)
        initialize();
    return {g_context->rmode->fbWidth, g_context->rmode->efbHeight};
}

} // namespace gexxo
