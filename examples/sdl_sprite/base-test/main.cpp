#include <coffee/core/CApplication>
#include <coffee/core/CFiles>
#include <coffee/core/CInput>

#include <coffee/CImage>

#include <coffee/CSDL2>
#include <coffee/sdl2/graphics/csdl2_basic_renderer.h>

using namespace Coffee;
using namespace CDisplay;

class SpriteTest : public SDL2Window,public SDL2EventHandler,public CDRendererBase
{
    // EventApplication interface
public:
    SpriteTest():
        CDRendererBase(0)
    {
    }

    void init(const CDProperties &props)
    {
        windowPreInit(props);
        inputPreInit();

        windowInit(props);
        inputInit();

        windowPostInit(props);
        inputPostInit();
    }
    void run()
    {
    }
    void cleanup()
    {
        inputTerminate();
        windowTerminate();
    }

    void eventHandleD(const CDEvent &e, c_cptr d)
    {
        SDL2EventHandler::eventHandleD(e,d);

        EventHandlers::WindowManagerCloseWindow(this,e,d);
    }

    void eventHandleI(const CIEvent& e, c_cptr data)
    {
        SDL2EventHandler::eventHandleI(e,data);

        EventHandlers::EscapeCloseWindow(this,e,data);
    }
};

using Sprites = SDL2SpriteRenderer;

int32 coffee_main(int32 argc, cstring_w* argv)
{
    SubsystemWrapper<SDL2::SDL2> sys1;
    C_UNUSED(sys1);

    CResources::FileResourcePrefix("sample_data/");

    SpriteTest test;
    test.init(GetDefaultVisual());
    Profiler::Profile("Window init");

    Sprites rend(&test);
    rend.init();
    Profiler::Profile("Renderer creation");
    Sprites::Renderer r = rend.createRenderer();
    Profiler::Profile("Context handle");
    Sprites::Texture t;
    rend.createTexture(r,1,&t,PixelFormat::RGBA8UI,ResourceAccess::Streaming,CSize(128,128));
    Profiler::Profile("Texture creation");

    {
        CResources::Resource texfile("ctest_hud/particle_sprite.png");
        CResources::FileMap(texfile);

        CStbImageLib::CStbImage img;
        CStbImageLib::LoadData(&img,&texfile);

        {
            CRGBA* data = (CRGBA*)rend.mapTexture(t);
            MemCpy(data,img.data,img.size.area()*img.bpp);
            rend.unmapTexture(t);
        }

        CStbImageLib::ImageFree(&img);

        CResources::FileUnmap(texfile);
    }
    Profiler::Profile("Texture load");

    CRect src(0,0,128,128);
    Sprites::Sprite sprite;
    rend.createSprite(t,src,&sprite);

    test.showWindow();
    rend.setClearColor(r,CRGBA(0,0,0));
    Profiler::Profile("Renderer state");
    while(!test.closeFlag())
    {
        rend.clearBuffer(r);
        rend.drawSprite(r,CPointF(0,0),CSizeF(1,1),sprite);

        rend.swapBuffers(r);
        test.pollEvents();
    }
    test.hideWindow();

    rend.terminate();
    test.cleanup();

    return 0;
}


COFFEE_APPLICATION_MAIN(coffee_main)