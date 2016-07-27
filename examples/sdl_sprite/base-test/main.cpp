#include <coffee/core/CApplication>
#include <coffee/core/CProfiling>
#include <coffee/core/CFiles>
#include <coffee/core/CInput>

#include <coffee/CImage>

#include <coffee/sdl2/CSDL2System>
#include <coffee/sdl2/CSDL2Dialog>
#include <coffee/sdl2/CSDL2SpriteWindow>

using namespace Coffee;
using namespace Display;

using BasicWindow = SDL2WindowHost;
using Sprites = SDL2SpriteRenderer;

const constexpr szptr num_points = 10;

CPointF sprite_pos[num_points] = {};
CSizeF sprite_scale = {1,1};

#ifndef COFFEE_USE_RTTI
SDL2WindowHost* window_host;
#endif

bool exit_flag = false;

void TouchInput_1(void*, const CIEvent& e, c_cptr d)
{
    if(e.type == CIEvent::MultiTouch)
    {
        const CIMTouchMotionEvent& tch = *(const CIMTouchMotionEvent*)d;
        if(tch.fingers==2)
        {
            scalar v = tch.dist;
            v /= Int16_Max;
            v += 1.f;
            CSizeF s = {v,v};
            sprite_scale.w *= s.w;
            sprite_scale.h *= s.h;
        }
    }
    else if(e.type == CIEvent::TouchMotion)
    {
        const CITouchMotionEvent& tch = *(const CITouchMotionEvent*)d;
        if(tch.finger>num_points)
            return;
        if(!tch.hover)
        {
            sprite_pos[tch.finger] = tch.origin;
            sprite_pos[tch.finger].x *= 1280;
            sprite_pos[tch.finger].y *= 720;
            sprite_pos[tch.finger].x -= 64;
            sprite_pos[tch.finger].y -= 64;
        }
    }else if(e.type == CIEvent::MouseMove)
    {
        const CIMouseMoveEvent& mbn = *(const CIMouseMoveEvent*)d;
        if(mbn.btn!=0)
        {
            sprite_pos[0] = mbn.origin;
            sprite_pos[0].x -= 64;
            sprite_pos[0].y -= 64;
        }
    }else if(e.type == CIEvent::TouchTap)
    {
        const CITouchTapEvent& tch = *(const CITouchTapEvent*)d;
        if(tch.finger>num_points)
            return;
        if(tch.pressed)
        {
            sprite_pos[tch.finger] = tch.pos;
            sprite_pos[tch.finger].x *= 1280;
            sprite_pos[tch.finger].y *= 720;
            sprite_pos[tch.finger].x -= 64;
            sprite_pos[tch.finger].y -= 64;
        }
    }else if(e.type == CIEvent::MouseButton)
    {
        const CIMouseButtonEvent& mbn = *(const CIMouseButtonEvent*)d;
        if(mbn.mod&CIMouseButtonEvent::Pressed)
        {
            sprite_pos[0] = mbn.pos;
            sprite_pos[0].x -= 64;
            sprite_pos[0].y -= 64;
        }
    }
}

void ExitHandler_1(void* ptr, const CIEvent& e, c_cptr d)
{
#ifdef COFFEE_USE_RTTI
    SDL2WindowHost* host = dynamic_cast<SDL2WindowHost*>((SDL2EventHandler*)ptr);
#else
    SDL2WindowHost* host = window_host;
#endif

    if(e.type == CIEvent::QuitSign)
    {
        host->closeWindow();
    }else if(e.type==CIEvent::Keyboard)
    {
        CIKeyEvent const* kev = (CIKeyEvent const*)d;
        if(kev->key==CK_Escape)
            host->closeWindow();
    }
}

int32 coffee_main(int32 argc, cstring_w* argv)
{
    SubsystemWrapper<SDL2::SDL2> sys1;
    C_UNUSED(sys1);

    /* Set file prefix, basically a cwd but only for resources */
    CResources::FileResourcePrefix("sample_data/ctest_hud/");

    /* Create a window host for the renderer */
    BasicWindow test;
    auto visual = GetDefaultVisual();
#ifdef COFFEE_ANDROID
    visual.flags = CDProperties::FullScreen;
#endif

#ifndef COFFEE_USE_RTTI
    window_host = &test;
#endif

    CString err;

    if(!test.init(visual,&err))
    {
        cDebug("Initialization error: {0}",err);
        return 1;
    }
    Profiler::Profile("Window init");

    /* Create a sprite renderer context */
    Sprites rend(&test);
    if(!rend.init(&err))
    {
        cDebug("Initialization error: {0}",err);
        return 1;
    }

    /* Create a renderer handle */
    Sprites::Renderer r = rend.createRenderer();
    Profiler::Profile("Context handle");

    /* Create a texture */
    Sprites::Texture t;
    rend.createTexture(r,1,&t,PixelFormat::RGBA8UI,ResourceAccess::Streaming,CSize(128,128));
    Profiler::Profile("Texture creation");

    {
        /* Map a texture into memory */
        CResources::Resource texfile("particle_sprite.png",false,
                                     ResourceAccess::SpecifyStorage|
                                     ResourceAccess::AssetFile|
                                     ResourceAccess::ReadOnly);

        cDebug("Opening texture: {0}",texfile.resource());

        CResources::FileMap(texfile);

        cDebug("Pointer to texture: {0}",(const byte_t*)texfile.data);

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
    CRect src(0,0,128,128);
    Sprites::Sprite sprite;
    rend.createSprite(t,src,&sprite);

    /* Show the window */
    test.showWindow();

    CRGBA clearCol = CRGBA(255,0,0);

    /* Set clear color for buffer */
    rend.setClearColor(r,clearCol);
    Profiler::Profile("Renderer state");
    /* Start rendering! */

    SDL2Dialog::InformationMessage("Leaving?","Hello there! Did you press the wrong button?");

    test.installEventHandler({TouchInput_1});
    test.installEventHandler({ExitHandler_1});

    while(!test.closeFlag())
    {
        rend.setClearColor(r,clearCol);

        rend.clearBuffer(r);
        for(szptr i=0;i<num_points;i++)
            rend.drawSprite(r,sprite_pos[i],sprite_scale,sprite);

        clearCol.r = (Time::CurrentTimestamp()*1000)%255;

        rend.swapBuffers(r);
        test.pollEvents();
    }

    rend.destroyTexture(1,&t);
    rend.destroyRenderer(r);

    test.hideWindow();

    rend.spritesTerminate();
    test.cleanup();

    return 0;
}


COFFEE_APPLICATION_MAIN(coffee_main)
