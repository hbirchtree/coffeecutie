#include <coffee/windowing/renderer/renderer.h>
#include <coffee/graphics/apis/CGLeamRHI>
#include <coffee/graphics/apis/CGLeam>
#include <coffee/core/CApplication>
#include <coffee/core/CInput>
#include <coffee/CGraphics>
#include <coffee/core/coffee.h>
#include <coffee/core/CFiles>
#include <coffee/core/CDebug>

using namespace Coffee;
using namespace Display;

using CDRenderer = CSDL2Renderer;

struct SharedData
{
    uint16 frame_count;
    Timestamp frame_ts;

    void* ptr;

    RHI::GLEAM::GLEAM_API::API_CONTEXT api;
};

using ELoop = EventLoopData<CDRenderer, SharedData>;

void setup_fun(CDRenderer& renderer, SharedData* data)
{
    renderer.setWindowTitle("GL extensions");
    cDebug("GL extensions: {0}",CGL::CGL_Shared_Debug::s_ExtensionList);
    cDebug("Framebuffer size: {0}, window size: {1}",
           renderer.framebufferSize(), renderer.windowSize());
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
    RHI::GLEAM::GLEAM_API::DefaultFramebuffer()
            .clear(0, {1.f, 1.f, 0.f, 0.1f});

    if(data->frame_ts <= Time::CurrentTimestamp())
    {
        cDebug("FPS: {0}", data->frame_count);
        data->frame_ts = Time::CurrentTimestamp() + 1;
        data->frame_count = 0;
    }
    data->frame_count++;

    renderer.pollEvents();
    renderer.swapBuffers();
}

void cleanup_fun(CDRenderer&, SharedData*)
{
}

int32 coffee_main(int32, cstring_w*)
{
    CString err;

    ELoop* globLoop = new ELoop{
            Display::CreateRendererUq(),
            MkUq<SharedData>(),
            setup_fun, loop_fun, cleanup_fun,
            ELoop::TimeLimited, {}};

    ELoop& eventloop = *globLoop;
    eventloop.time.max = 10;

    CDProperties visual = GetDefaultVisual<RHI::GLEAM::GLEAM_API>();
    visual.flags ^= CDProperties::Windowed;
    visual.flags |= CDProperties::WindowedFullScreen;

    cDebug("Visual: {0}", visual.gl.version);

    eventloop.r().installEventHandler(
    {
                    EventHandlers::EscapeCloseWindow<CDRenderer>,
                    nullptr, &eventloop.r()
                });
    eventloop.r().installEventHandler(
    {
                    EventHandlers::ExitOnQuitSignal<CDRenderer>,
                    nullptr, &eventloop.r()
                });
    eventloop.r().installEventHandler(
    {
                    EventHandlers::WindowManagerCloseWindow<CDRenderer>,
                    nullptr, &eventloop.r()
                });
    eventloop.r().installEventHandler(
    {
                    EventHandlers::ResizeWindowUniversal<RHI::GLEAM::GLEAM_API>,
                    nullptr, &eventloop.r()
                });

    int32 stat = CDRenderer::execEventLoop(eventloop, visual, err);

    if(stat != 0)
        cDebug("Init error: {0}", err);

    return stat;
}

COFFEE_APPLICATION_MAIN(coffee_main)
