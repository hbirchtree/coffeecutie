#include <coffee/sdl2/CSDL2GLRenderer>
#include <coffee/graphics/apis/CGLeam>
#include <coffee/core/CApplication>
#include <coffee/core/CInput>
#include <coffee/CGraphics>
#include <coffee/core/coffee.h>

using namespace Coffee;
using namespace Display;

#ifdef COFFEE_GLEAM_DESKTOP
using GL = CGL::CGL43;
#else
using GL = CGL::CGLES30;
#endif

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
        EventHandlers::ResizeWindow<GL>(e,data);
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
    CResources::FileResourcePrefix("sample_data/");

    CString err;
    CDRenderer renderer;

    CDProperties visual = GetDefaultVisual(3,3);
    if(PlatformData::IsGLES())
    {
        visual.gl.version.major = 2;
        visual.gl.version.minor = 0;
    }

    SharedData share_data = {};

    auto setup_fun = [](CDRenderer&, SharedData*)
    {
        cDebug("GL extensions: {0}",GL::Debug::s_ExtensionList);
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
    {&renderer, &share_data,
     setup_fun, loop_fun, cleanup_fun};

#if defined(__EMSCRIPTEN__)
    emscripten_set_main_loop_arg(emscripten_middleman<CDRenderer,SharedData>, &eventloop, 0, 0);
#endif

    if(!renderer.init(visual,&err))
    {
        cDebug("Initialization error: {0}",err);
        return 1;
    }


    eventloop.setup(*eventloop.renderer, eventloop.data);
#if defined(__EMSCRIPTEN__)
    emscripten_resume_main_loop();
#else
    while(!eventloop.renderer->closeFlag())
        emscripten_middleman<CDRenderer,SharedData>(&eventloop);
#endif
    eventloop.cleanup(*eventloop.renderer, eventloop.data);

    renderer.cleanup();

    return 0;
}

COFFEE_APPLICATION_MAIN(coffee_main)
