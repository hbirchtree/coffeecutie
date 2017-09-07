#include <coffee/windowing/renderer/renderer.h>
#include <coffee/graphics/apis/CGLeamRHI>
#include <coffee/graphics/apis/CGLeam>
#include <coffee/core/CApplication>
#include <coffee/core/CInput>
#include <coffee/CGraphics>
#include <coffee/core/coffee.h>
#include <coffee/core/CFiles>

using namespace Coffee;
using namespace Display;

class CDRenderer : public CSDL2Renderer
{
public:
    CDRenderer():
        CSDL2Renderer(0)
    {
    }

    void run()
    {
    }
    void eventHandleD(const Display::CDEvent &e, c_cptr data)
    {
        CSDL2Renderer::eventHandleD(e,data);

        EventHandlers::WindowManagerCloseWindow(this,e,data);
        EventHandlers::ResizeWindowUniversal<RHI::GLEAM::GLEAM_API>(e,data);

        cDebug("Event: {0}", C_CAST<uint32>(e.type));
    }
    void eventHandleI(const CIEvent &e, c_cptr data)
    {
        CSDL2Renderer::eventHandleI(e,data);

        EventHandlers::EscapeCloseWindow(this,e,data);
        EventHandlers::ExitOnQuitSignal(this, e, data);

        cDebug("Event: {0}", C_CAST<uint32>(e.type));
    }

    CQuat r_view;
};

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
}

void loop_fun(CDRenderer& renderer, SharedData* data)
{
    if(renderer.contextTime() > 5.0)
        renderer.closeWindow();

    RHI::GLEAM::GLEAM_API::DefaultFramebuffer().clear(0, {1.f, 1.f, 0.f, 0.1f});

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
    SetPrintingVerbosity(8);
    FileResourcePrefix("sample_data/");

    CString err;

    ELoop* globLoop = new ELoop{
            new CDRenderer,
            new SharedData,
            setup_fun, loop_fun, cleanup_fun,
            ELoop::TimeLimited, {}};

    ELoop& eventloop = *globLoop;
    eventloop.time.max = 3;

    CDProperties visual = GetDefaultVisual<RHI::GLEAM::GLEAM_API>();
    visual.flags ^= CDProperties::Windowed;
    visual.flags |= CDProperties::WindowedFullScreen;

    int32 stat = CDRenderer::execEventLoop(eventloop, visual, err);

    if(stat != 0)
        cDebug("Init error: {0}", err);

    return stat;
}

COFFEE_APPLICATION_MAIN(coffee_main)
