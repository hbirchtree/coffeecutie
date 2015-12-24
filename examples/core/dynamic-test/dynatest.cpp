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

extern "C"{

CDRendererBase* CoffeeLoader();

}

CDRendererBase* CoffeeLoader()
{
    return new CDRenderer();
}
