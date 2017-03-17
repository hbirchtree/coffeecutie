#include <coffee/sdl2/CSDL2GLRenderer>
#include <coffee/graphics/apis/CGLeamRHI>
#include <coffee/graphics/apis/CGLeam>
#include <coffee/core/CApplication>
#include <coffee/core/CInput>
#include <coffee/CGraphics>
#include <coffee/core/coffee.h>
#include <coffee/core/CFiles>

using namespace Coffee;
using namespace Display;
using namespace CResources;

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
    }
    void eventHandleI(const CIEvent &e, c_cptr data)
    {
        CSDL2Renderer::eventHandleI(e,data);

        EventHandlers::EscapeCloseWindow(this,e,data);
        EventHandlers::RotateView(r_view,e,data);
    }

    CQuat r_view;
};

template<typename R, typename D>
void emscripten_middleman(void* arg)
{
    auto eventloop = C_FCAST< EventLoopData<R,D>* >(arg);

    eventloop->loop(*eventloop->renderer, eventloop->data);
}

struct SharedData
{
    void* ptr;
};

int32 coffee_main(int32, cstring_w*)
{
    FileResourcePrefix("sample_data/");

    CString err;
    CDRenderer renderer;

    CDProperties visual = GetDefaultVisual<RHI::GLEAM::GLEAM_API>();

    SharedData share_data = {};

    auto setup_fun = [](CDRenderer&, SharedData*)
    {
        cDebug("GL extensions: {0}",CGL::CGL_Shared_Debug::s_ExtensionList);
    };
    auto loop_fun = [](CDRenderer& renderer, SharedData*)
    {
        renderer.pollEvents();
        renderer.swapBuffers();
    };
    auto cleanup_fun = [](CDRenderer&, SharedData*)
    {

    };

    EventLoopData<CDRenderer, SharedData> eventloop =
    {&renderer, &share_data, setup_fun, loop_fun, cleanup_fun, 0};

    int32 stat = CDRenderer::execEventLoop(eventloop, visual, err);
    if(stat != 0)
        cDebug("Init error: {0}", err);
    return stat;
}

COFFEE_APPLICATION_MAIN(coffee_main)
