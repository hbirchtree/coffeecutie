#include <coffee/core/CApplication>
#include <coffee/core/CFiles>
#include <coffee/sdl2/CSDL2Dialog>
#include <coffee/sdl2/CSDL2System>
#include <coffee/sdl2/CSDL2SpriteWindow>
#include <coffee/sdl2/CSDL2WindowHost>
#include <coffee/core/input/eventhandlers.h>

#include <coffee/image/cimage.h>


using namespace Coffee;
using namespace Display;

using RSC = CResources::Resource;
using BasicWindow = SDL2WindowHost;
using Sprites = SDL2SpriteRenderer;


Sprites::Texture sprite_load(Sprites* instance, Sprites::Renderer* renderer,
                             cstring source, Sprites::SpriteSource* size)
{
    Sprites::Texture out;

    RSC analog_rsc(source,
                   ResourceAccess::SpecifyStorage
                   |ResourceAccess::AssetFile);
    if(!CResources::FileMap(analog_rsc))
    {
        SDL2Dialog::ErrorMessage("Failed to load resource","Couldn't find texture");
        return {};
    }
    CStbImageLib::CStbImage img;
    CStbImageLib::LoadData(&img,&analog_rsc,PixCmp::RGBA);

    instance->createTexture(*renderer,1,&out,
                            PixelFormat::RGBA8,
                            ResourceAccess::Streaming,
                            {img.size.w,img.size.h});

    if(size)
    {
        size->w = img.size.w;
        size->h = img.size.h;
    }

    CRGBA* data = (CRGBA*)instance->mapTexture(out);
    MemCpy(data,img.data,img.size.area()*img.bpp);
    instance->unmapTexture(out);

    CStbImageLib::ImageFree(&img);
    CResources::FileUnmap(analog_rsc);

    return out;
}

int32 coffee_main(int32, cstring_w*)
{
    SubsystemWrapper<SDL2::SDL2> sys1;
    C_UNUSED(sys1);

    CResources::FileResourcePrefix("sample_data/input-sprites/");

    auto visual = GetDefaultVisual();

    CString err;

    BasicWindow winhost;
    if(!winhost.init(visual,&err))
    {
        SDL2Dialog::ErrorMessage("Failed to create window",
                                 err.c_str());
        return 1;
    }

    /* Attaching the sprite instance to the window */
    Sprites rend(&winhost);
    if(!rend.init(&err))
    {
        SDL2Dialog::ErrorMessage("Failed to initialize renderer",
                                 err.c_str());
        return 1;
    }

    /* Create a rendering instance */
    Sprites::Renderer inst = rend.createRenderer();

    rend.setClearColor(inst,CRGBA(0,0,0,255));
    rend.clearBuffer(inst);

    /* Create sprites */
    Sprites::Texture controller_atlas = sprite_load(&rend,&inst,"atlas.png",nullptr);

    Sprites::Sprite analog_sprite;
    Sprites::Sprite analog_back_sprite;
    Sprites::Sprite trigger_sprite;
    Sprites::Sprite trigger_back_sprite;

    Sprites::Sprite button_a_sprite;
    Sprites::Sprite button_x_sprite;
    Sprites::Sprite button_b_sprite;
    Sprites::Sprite button_y_sprite;

    Sprites::Sprite button_ap_sprite;
    Sprites::Sprite button_xp_sprite;
    Sprites::Sprite button_bp_sprite;
    Sprites::Sprite button_yp_sprite;

    rend.createSprite(controller_atlas,{256,0,128,128},&analog_sprite);
    rend.createSprite(controller_atlas,{0,0,256,256},&analog_back_sprite);

    rend.createSprite(controller_atlas,{128,256,96,32},&trigger_sprite);
    rend.createSprite(controller_atlas,{0,256,96,256},&trigger_back_sprite);

    rend.createSprite(controller_atlas,{384,0,128,128},&button_a_sprite);
    rend.createSprite(controller_atlas,{384,128,128,128},&button_x_sprite);
    rend.createSprite(controller_atlas,{640,0,128,128},&button_b_sprite);
    rend.createSprite(controller_atlas,{640,128,128,128},&button_y_sprite);

    rend.createSprite(controller_atlas,{512,0,128,128},&button_ap_sprite);
    rend.createSprite(controller_atlas,{512,128,128,128},&button_xp_sprite);
    rend.createSprite(controller_atlas,{768,0,128,128},&button_bp_sprite);
    rend.createSprite(controller_atlas,{768,128,128,128},&button_yp_sprite);

    /* Install standard event handlers */
    winhost.installEventHandler({EventHandlers::EscapeCloseWindow<BasicWindow>,nullptr,&winhost});
    winhost.installEventHandler({EventHandlers::WindowManagerCloseWindow<BasicWindow>,nullptr,&winhost});

    while(!winhost.closeFlag())
    {
        rend.setClearColor(inst,CRGBA(0,0,0,255));
        rend.clearBuffer(inst);

        {
            auto ctl = winhost.getControllerState(0);
            rend.drawSprite(inst,{0,256+128},{1,1},analog_back_sprite);
            rend.drawSprite(inst,{scalar(scalar(ctl.axes.e.l_x)/Int16_Max*128.+64.),
                                  scalar(scalar(ctl.axes.e.l_y)/Int16_Max*128.+64.+256.+128.)},
                                 {1,1},analog_sprite);

            rend.drawSprite(inst,{512,256+128.},{1,1},analog_back_sprite);
            rend.drawSprite(inst,{scalar(scalar(ctl.axes.e.r_x)/Int16_Max*128.+64.+512.),
                                  scalar(scalar(ctl.axes.e.r_y)/Int16_Max*128.+64.+256.+128.)},
                                 {1,1},analog_sprite);

            rend.drawSprite(inst,{0,0},{1,1},trigger_back_sprite);
            rend.drawSprite(inst,{0.,
                                  scalar(scalar(ctl.axes.e.t_l)/Int16_Max*220.)},
                                 {1,1},trigger_sprite);

            rend.drawSprite(inst,{640,0},{1,1},trigger_back_sprite);
            rend.drawSprite(inst,{640,
                                  scalar(scalar(ctl.axes.e.t_r)/Int16_Max*220.)},
                                 {1,1},trigger_sprite);

            if(!ctl.buttons.e.a)
                rend.drawSprite(inst,{320,256+96-16},{1,1},button_a_sprite);
            else
                rend.drawSprite(inst,{320,256+96-16},{1,1},button_ap_sprite);
            if(!ctl.buttons.e.x)
                rend.drawSprite(inst,{192+16+16,128+96},{1,1},button_x_sprite);
            else
                rend.drawSprite(inst,{192+16+16,128+96},{1,1},button_xp_sprite);
            if(!ctl.buttons.e.b)
                rend.drawSprite(inst,{432-16,128+96},{1,1},button_b_sprite);
            else
                rend.drawSprite(inst,{432-16,128+96},{1,1},button_bp_sprite);
            if(!ctl.buttons.e.y)
                rend.drawSprite(inst,{320,0+96+16},{1,1},button_y_sprite);
            else
                rend.drawSprite(inst,{320,0+96+16},{1,1},button_yp_sprite);

            if(ctl.buttons.e.guide)
                cDebug("GUIDE ME");
        }

        rend.swapBuffers(inst);
        winhost.pollEvents();
    }

    rend.destroyTexture(1,&controller_atlas);
    rend.destroyRenderer(inst);

    rend.cleanup();
    winhost.cleanup();

    return 0;
}

COFFEE_APPLICATION_MAIN(coffee_main);

