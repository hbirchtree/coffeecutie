#include <coffee/CCore>
#include <coffee/graphics/apis/CGLeam>

using namespace Coffee;
using namespace CDisplay;

class CDRenderer : public Coffee::CDisplay::CGLeamRenderer
{
public:
    CDRenderer()
        : CGLeamRenderer(0)
    {
    }

    void run()
    {
        CVec4 clearcol(0.0);
        clearcol.a() = 1.0;

        this->showWindow();
        while(!closeFlag())
        {
            clearcol.r() = CMath::sin(this->contextTime()+0.5);
            clearcol.g() = CMath::sin(this->contextTime()+5.0);
            clearcol.b() = CMath::sin(this->contextTime()+50.0);

            clearcol = normalize(clearcol);

            CGL::CGL33::ClearBufferfv(true,0,clearcol);


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
        switch(e.type)
        {
        case CIEvent::Keyboard:
        {
            const CIKeyEvent* kev = (const CIKeyEvent*)data;
            switch(kev->key)
            {
            case CK_Escape:
                this->closeWindow();
                break;
            default:
                break;
            }
            break;
        }
        default:
            break;
        }
    }
};

int32 coffee_main(int32 argc, byte_t** argv)
{
    CDRendererBase *renderer = new CDRenderer();
    CDWindowProperties props = coffee_get_default_visual();
    props.contextProperties.flags = props.contextProperties.flags|
            CGLContextProperties::GLDebug|
            CGLContextProperties::GLVSync;
    renderer->init(props);
    renderer->run();
    renderer->cleanup();
    delete renderer;
    return 0;
}

COFFEE_APPLICATION_MAIN(coffee_main)
