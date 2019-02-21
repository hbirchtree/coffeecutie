#include <coffee/core/CUnitTesting>
#include <coffee/core/coffee.h>
#include <coffee/core/input/eventhandlers.h>
#include <coffee/windowing/renderer/renderer.h>

#if defined(COFFEE_GEKKO)
#include <malloc.h>
#include <ogcsys.h>
#include <stdlib.h>
#endif

#if !defined(COFFEE_GEKKO)
#include <coffee/graphics/apis/CGLeamRHI>
using API = Coffee::RHI::GLEAM::GLEAM_API;
#else
#include <coffee/interfaces/cgraphics_api.h>
#include <gccore.h>
using API = Coffee::RHI::NullAPI;
#endif

struct WindowData
{
    void* data = nullptr;
};

using namespace Coffee;
using namespace Display;

using R = CSDL2Renderer;
using D = WindowData;

using EventLoop = EventLoopData<RendererInterface, ::WindowData>;

#if defined(COFFEE_GEKKO)

static i16 vertices[] ATTRIBUTE_ALIGN(32) = {
    0,
    15,
    0,
    -15,
    -15,
    0,
    15,
    -15,
    0,
};

static u8 colors[] ATTRIBUTE_ALIGN(32) = {
    255,
    0,
    0,
    255,
    0,
    255,
    0,
    255,
    0,
    0,
    255,
    255,
};

extern vu8 readyForCopy;

extern GXRModeObj* gamecube_rmode;
extern void*       gamecube_xfb;

Mtx view, projection;
#endif

::EventLoop* GenELoop()
{
    return new ::EventLoop{
        Display::CreateRendererUq(),
        MkUq<WindowData>(),
        [](R& r, D*) {
            cDebug("Setup");

            printf(
                "Screen size: %ix%i\n",
                r.framebufferSize().w,
                r.framebufferSize().h);
#if defined(COFFEE_GEKKO)
            GXColor backgroundColor = {127, 0, 0, 255};

            GX_SetCopyClear(backgroundColor, 0x00ffffff);

            GX_SetDispCopyYScale(
                (f32)gamecube_rmode->xfbHeight /
                (f32)gamecube_rmode->efbHeight);
            GX_SetScissor(
                0, 0, gamecube_rmode->fbWidth, gamecube_rmode->efbHeight);
            GX_SetDispCopySrc(
                0, 0, gamecube_rmode->fbWidth, gamecube_rmode->efbHeight);
            GX_SetDispCopyDst(
                gamecube_rmode->fbWidth, gamecube_rmode->xfbHeight);
            GX_SetCopyFilter(
                gamecube_rmode->aa,
                gamecube_rmode->sample_pattern,
                GX_TRUE,
                gamecube_rmode->vfilter);

            auto screenMode = gamecube_rmode;

            GX_SetFieldMode(
                gamecube_rmode->field_rendering,
                ((screenMode->viHeight == 2 * screenMode->xfbHeight)
                     ? GX_ENABLE
                     : GX_DISABLE));

            GX_CopyDisp(gamecube_xfb, GX_TRUE);
            GX_SetDispCopyGamma(GX_GM_1_0);

            guVector camera = {0.0F, 0.0F, 0.0F};
            guVector up     = {0.0F, 1.0F, 0.0F};
            guVector look   = {0.0F, 0.0F, -1.0F};

            guPerspective(projection, 60, 1.33F, 10.0F, 300.0F);
            GX_LoadProjectionMtx(projection, GX_PERSPECTIVE);

            GX_ClearVtxDesc();
            GX_SetVtxDesc(GX_VA_POS, GX_INDEX8);
            GX_SetVtxDesc(GX_VA_CLR0, GX_INDEX8);
            GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_S16, 0);
            GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_CLR0, GX_CLR_RGBA, GX_RGBA8, 0);
            GX_SetArray(GX_VA_POS, vertices, 3 * sizeof(s16));
            GX_SetArray(GX_VA_CLR0, colors, 4 * sizeof(u8));
            GX_SetNumChans(1);
            GX_SetNumTexGens(0);
            GX_SetTevOrder(
                GX_TEVSTAGE0, GX_TEXCOORDNULL, GX_TEXMAP_NULL, GX_COLOR0A0);
            GX_SetTevOp(GX_TEVSTAGE0, GX_PASSCLR);
#endif
        },
        [](R& r, D*) {
#if defined(COFFEE_GEKKO)
            static int i = 0;

            guVector camera = {0.0F, 0.0F, 0.0F};
            guVector up     = {0.0F, 1.0F, 0.0F};
            guVector look   = {0.0F, 0.0F, -1.0F};
            guLookAt(view, &camera, &up, &look);
            GX_SetViewport(
                0, 0, gamecube_rmode->fbWidth, gamecube_rmode->efbHeight, 0, 1);
            GX_InvVtxCache();
            GX_InvalidateTexAll();
            Mtx modelView;

            i++;

            guMtxIdentity(modelView);
            guMtxTransApply(modelView, modelView, 0.0F, 0.0F, -((i % 50) + 25));
            guMtxConcat(view, modelView, modelView);

            GX_LoadPosMtxImm(modelView, GX_PNMTX0);

            GX_Begin(GX_TRIANGLES, GX_VTXFMT0, 3);

            GX_Position1x8(0);
            GX_Color1x8(0);
            GX_Position1x8(1);
            GX_Color1x8(1);
            GX_Position1x8(2);
            GX_Color1x8(2);

            GX_End();

#endif
            r.pollEvents();
            r.swapBuffers();
        },
        [](R&, D*) { cDebug("Cleanup"); },
        0,
        {},
        {}};
}

#if defined(COFFEE_GEKKO)
void gc_memorymap()
{
    printf(
        "MEM1:\t%p+%u\nExecutable segment: %p+%u\n",
        SYS_GetArena1Lo(),
        SYS_GetArena1Size(),
        nullptr,
        0);
}
#endif

bool basic_window_test()
{
    auto win = Display::CreateRendererUq();

    ::EventLoop* eld = GenELoop();

    eld->flags    = ::EventLoop::TimeLimited;
    eld->time.max = 1;

    eld->r().installEventHandler(
        {EventHandlers::ExitOnQuitSignal<R>, nullptr, &eld->r()});

    auto visual = GetDefaultVisual<API>();

    CString err;
    int     status = EventApplication::execEventLoop(*eld, visual, err);

#if defined(COFFEE_GEKKO)
    printf("Yeeeeh m8\n");
    atexit(gc_memorymap);

    printf("Arena1: %p+%u\n", SYS_GetArena1Lo(), SYS_GetArena1Size());
    printf("Executable code location: %p\n", basic_window_test);

    printf("Bus speed: %u\n", *((unsigned int*)0x800000F8));
    printf("CPU speed: %u\n", *((unsigned int*)0x800000FC));
    printf("System type: 0x%p\n", *((unsigned int*)0x8000002C));

    auto q = RuntimeQueue::CreateNewThreadQueue("TEST_THREAD");

    if(RuntimeQueue::QueuePeriodic(q, Chrono::seconds(2), []() {
           auto gc_time = time(NULL);
           printf("DERP DERP: %s\n", ctime(&gc_time));
       }) == 0)
        printf("Failed to enqueue\n");
    auto gc_time = time(NULL);
    printf("Schedule time: %s", ctime(&gc_time));
#endif

    return status == 0;
}

COFFEE_TESTS_BEGIN(1)

    {basic_window_test, "Basic windowing", nullptr, false, false}

COFFEE_TESTS_END()
