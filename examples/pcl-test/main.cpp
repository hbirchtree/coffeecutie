#include <coffee/CCore>
#include <coffee/CGraphics>

using namespace Coffee;
using namespace CDisplay;

class CDRenderer : public Coffee::CDisplay::CGLBindingRenderer
{
public:
    CDRenderer()
        : CGLBindingRenderer(0)
    {
    }

    void run()
    {
        //Write code here

        this->showWindow();
        while(!closeFlag())
        {
            coffee_graphics_clear(CClearFlag::Color);

            //Write code here

            this->pollEvents();
            this->swapBuffers();
        }

        //Write code here
    }
    void eventHandleD(const CDisplay::CDEvent &e, c_cptr data)
    {
        CSDL2Renderer::eventHandleD(e,data);
    }
    void eventHandleI(const CIEvent &e, c_cptr data)
    {
        CSDL2Renderer::eventHandleI(e,data);
    }
};

int32 coffee_main(int32 argc, byte_t** argv)
{
    CDRendererBase *renderer = new CDRenderer();
    renderer->init(coffee_get_default_visual());
    renderer->run();
    renderer->cleanup();
    return 0;
}

COFFEE_APPLICATION_MAIN(coffee_main)
