#include <coffee/core/CApplication>
#include <coffee/core/CFiles>
#include <coffee/core/CInput>

#include <coffee/core/plat/plat_file.h>

#include <coffee/CImage>

#include <coffee/sdl2/CSDL2System>
#include <coffee/sdl2/CSDL2SpriteWindow>

using namespace Coffee;
using namespace CDisplay;

using BasicWindow = SDL2WindowHost;
using Sprites = SDL2SpriteRenderer;

int32 coffee_main(int32 argc, cstring_w* argv)
{
    SubsystemWrapper<SDL2::SDL2> sys1;
    C_UNUSED(sys1);

    /* Set file prefix, basically a cwd but only for resources */
    CResources::FileResourcePrefix("sample_data/");

    /* Create a window host for the renderer */
    BasicWindow test;
    test.init(GetDefaultVisual());
    Profiler::Profile("Window init");

    /* Create a sprite renderer context */
    Sprites rend(&test);
    rend.init();

    /* Create a renderer handle */
    Sprites::Renderer r = rend.createRenderer();
    Profiler::Profile("Context handle");

    /* Create a texture */
    Sprites::Texture t;
    rend.createTexture(r,1,&t,PixelFormat::RGBA8UI,ResourceAccess::Streaming,CSize(128,128));
    Profiler::Profile("Texture creation");

    {
        /* Map a texture into memory */
        CResources::Resource texfile("ctest_hud/particle_sprite.png");
        CResources::FileMap(texfile);

        /* Decode file to RGBA data */
        CStbImageLib::CStbImage img;
        CStbImageLib::LoadData(&img,&texfile);

        /* Copy texture into texture memory */
        {
            CRGBA* data = (CRGBA*)rend.mapTexture(t);
            MemCpy(data,img.data,img.size.area()*img.bpp);
            rend.unmapTexture(t);
        }

        /* Clean up */
        CStbImageLib::ImageFree(&img);
        CResources::FileUnmap(texfile);
    }
    Profiler::Profile("Texture load");

    /* Create a sprite from the texture */
    CRect src(64,64,64,64);
    Sprites::Sprite sprite;
    rend.createSprite(t,src,&sprite);

    /* Show the window */
    test.showWindow();
    /* Set clear color for buffer */
    rend.setClearColor(r,CRGBA(0,0,0));
    Profiler::Profile("Renderer state");
    /* Start rendering! */
    while(!test.closeFlag())
    {
        rend.clearBuffer(r);
        rend.drawSprite(r,CPointF(0,0),CSizeF(1,1),sprite);

        rend.swapBuffers(r);
        test.pollEvents();
    }
    test.hideWindow();

    rend.spritesTerminate();
    test.cleanup();

    return 0;
}


COFFEE_APPLICATION_MAIN(coffee_main)
