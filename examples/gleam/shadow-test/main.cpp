#include <coffee/core/CApplication>
#include <coffee/core/CFiles>
#include <coffee/core/CInput>
#include <coffee/core/coffee.h>
#include <coffee/core/platform_data.h>
#include <coffee/graphics/apis/CGLeam>
#include <coffee/graphics/apis/CGLeamRHI>
#include <coffee/interfaces/full_launcher.h>
#include <coffee/strings/info.h>
#include <coffee/strings/libc_types.h>
#include <coffee/windowing/renderer/renderer.h>
#include <peripherals/stl/time_types.h>

#include <coffee/core/CDebug>

using namespace Coffee;
using namespace Display;

using CDRenderer = CSDL2Renderer;

struct SharedData
{
    uint16    frame_count;
    Timestamp frame_ts;

    void* ptr;

    RHI::GLEAM::GLEAM_API::API_CONTEXT api;
};

using ELoop   = EventLoopData<CDRenderer, SharedData>;
using CGL_DBG = CGL::CGL_Shared_Debug;

void setup_fun(CDRenderer& renderer, SharedData* data)
{
    renderer.setWindowTitle("GL extensions");

    CGL_DBG::Context c;
    CGL_DBG::GetExtensions(c);

    cDebug("GL extensions: {0}", c.extensionList);
    cDebug(
        "Framebuffer size: {0}, window size: {1}",
        renderer.framebufferSize(),
        renderer.windowSize());
    cDebug("Framebuffer scale: {0}", PlatformData::DeviceDPI());
    cDebug("Monitor: {0}", renderer.monitor());

    data->api = RHI::GLEAM::GLEAM_API::GetLoadAPI();
    if(!data->api(PlatformData::IsDebug()))
    {
        cDebug("Failed to initialize graphics API");
    }

    renderer.popErrorMessage(Severity::Information, "Hello!", "Goodbye");

    renderer.showWindow();
}

void loop_fun(CDRenderer& renderer, SharedData* data)
{
    RHI::GLEAM::GLEAM_API::DefaultFramebuffer().clear(0, {1.f, 1.f, 0.f, 0.1f});

    if(data->frame_ts <= Time<>::CurrentTimestamp())
    {
        cDebug("FPS: {0}", data->frame_count);
        data->frame_ts    = Time<>::CurrentTimestamp() + 1;
        data->frame_count = 0;
    }
    data->frame_count++;

    renderer.pollEvents();
    renderer.swapBuffers();
}

void cleanup_fun(CDRenderer&, SharedData*)
{
}

i32 coffee_main(i32, cstring_w*)
{
    RuntimeQueue::CreateNewQueue("Main");

    CString err;

    return AutoExec<RHI::GLEAM::GLEAM_API, CDRenderer, SharedData>(
        [](CDRenderer& r, SharedData*, Display::Properties& visual) {
            visual.flags ^= Properties::Windowed;
            visual.flags |= Properties::WindowedFullScreen;

            r.installEventHandler(
                {EventHandlers::EscapeCloseWindow<CDRenderer>, nullptr, &r});
            r.installEventHandler(
                {EventHandlers::ExitOnQuitSignal<CDRenderer>, nullptr, &r});
            r.installEventHandler(
                {EventHandlers::WindowManagerCloseWindow<CDRenderer>,
                 nullptr,
                 &r});
            r.installEventHandler(
                {EventHandlers::ResizeWindowUniversal<RHI::GLEAM::GLEAM_API>,
                 nullptr,
                 &r});
        },
        setup_fun,
        loop_fun,
        cleanup_fun);
}

COFFEE_APPLICATION_MAIN(coffee_main)
