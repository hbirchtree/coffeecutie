#include <coffee/sdl2/CSDL2GLRenderer>
#include <coffee/graphics_apis/CGLeam>
#include <coffee/core/CApplication>
#include <coffee/core/CInput>
#include <coffee/CGraphics>

using namespace Coffee;
using namespace CDisplay;

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
		cDebug("GL extensions: {0}",GL::Debug::s_ExtensionList);

        while(!this->closeFlag())
        {
            this->pollEvents();
            this->swapBuffers();
        }
    }
    void eventHandleD(const CDisplay::CDEvent &e, c_cptr data)
    {
        CSDL2Renderer::eventHandleD(e,data);

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

int32 coffee_main(int32, cstring_w*)
{
    CResources::FileResourcePrefix("sample_data/");

    CString err;
    CDRenderer renderer;
#ifdef COFFEE_GLEAM_DESKTOP
    if(!renderer.init(GetDefaultVisual(3,3),&err))
    {
        cDebug("Initialization error: {0}",err);
        return 1;
    }
#else
    if(!renderer.init(GetDefaultVisual(3,0),&err))
    {
        cDebug("Initialization error: {0}",err);
        return 1;
    }
#endif
    renderer.run();
    renderer.cleanup();

    return 0;
}

COFFEE_APPLICATION_MAIN(coffee_main)
