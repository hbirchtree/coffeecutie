#include <coffee/CCore>
#include <coffee/graphics_apis/CGLeam>
#include <coffee/CGraphics>

using namespace Coffee;
using namespace CDisplay;

#ifdef COFFEE_GLEAM_DESKTOP
using GL = CGL::CGL43;
#else
using GL = CGL::CGLES30;
#endif

class CDRenderer : public Coffee::CDisplay::CGLeamRenderer
{
public:
    CDRenderer()
        : CGLeamRenderer(0)
    {
    }

    void run()
    {
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

    CDRenderer renderer;
    renderer.init(GetDefaultVisual(3,3));
    renderer.run();
    renderer.cleanup();

    return 0;
}

COFFEE_APPLICATION_MAIN(coffee_main)
