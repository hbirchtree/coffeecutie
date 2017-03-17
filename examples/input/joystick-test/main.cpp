#include <coffee/core/CApplication>
#include <coffee/core/CFiles>
#include <coffee/sdl2/CSDL2Dialog>
#include <coffee/sdl2/CSDL2System>
#include <coffee/sdl2/CSDL2SpriteWindow>
#include <coffee/sdl2/CSDL2WindowHost>
#include <coffee/core/input/eventhandlers.h>
#include <coffee/core/CDebug>

#include <coffee/core/types/map.h>
#include <coffee/image/cimage.h>

#include <coffee/graphics/apis/CGLeamRHI>

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

    CRGBA* data = C_CAST<CRGBA*>(instance->mapTexture(out));
    MemCpy(data,img.data, C_CAST<szptr>(img.size.area()*img.bpp));
    instance->unmapTexture(out);

    CStbImageLib::ImageFree(&img);
    CResources::FileUnmap(analog_rsc);

    return out;
}

void SDL_Handler(void*, SDL_Event* const& ev, c_cptr)
{
//    cDebug("Event received: {0}", ev->type);
}

int32 coffee_main(int32, cstring_w*)
{
    SubsystemWrapper<SDL2::SDL2> sys1;
    C_UNUSED(sys1);

    CResources::FileResourcePrefix("sample_data/input-sprites/");

    int32 majGL = 2, minGL = 0;
    RHI::GLEAM::GLEAM_API::GetDefaultVersion(majGL, minGL);
    auto visual = GetDefaultVisual(majGL, minGL);
    RHI::GLEAM::GLEAM_API::GetDefaultProperties(visual);

    CString err;

    BasicWindow winhost;
    if(!winhost.init(visual,&err))
    {
        SDL2Dialog::ErrorMessage("Failed to create window",
                                 err.c_str());
        return 1;
    }

    winhost.installEventHandler({SDL_Handler, nullptr, nullptr});

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

    Sprites::Sprite menu_sprite;
    Sprites::Sprite menu_p_sprite;

    Sprites::Sprite shoulderl_sprite;
    Sprites::Sprite shoulderl_p_sprite;

    Sprites::Sprite shoulderr_sprite;
    Sprites::Sprite shoulderr_p_sprite;

    Sprites::Sprite button_a_sprite;
    Sprites::Sprite button_x_sprite;
    Sprites::Sprite button_b_sprite;
    Sprites::Sprite button_y_sprite;

    Sprites::Sprite button_ap_sprite;
    Sprites::Sprite button_xp_sprite;
    Sprites::Sprite button_bp_sprite;
    Sprites::Sprite button_yp_sprite;

    Sprites::Sprite dpad_bg_sprite;
    Sprites::Sprite dpad_up_sprite;
    Sprites::Sprite dpad_dp_sprite;
    Sprites::Sprite dpad_lp_sprite;
    Sprites::Sprite dpad_rp_sprite;

    Sprites::Sprite empty_sprite;

    auto bind_sprite = [&](Sprites::SpriteSource const& src, Sprites::Sprite& sprite)
    {
        rend.createSprite(controller_atlas, src, &sprite);
    };

    _cbasic_static_map<Sprites::SpriteSource,Sprites::Sprite&,23> sprite_map = {
        {{256,0,128,128},analog_sprite},

        {{256,128,96,64},menu_sprite},
        {{256,192,96,64},menu_p_sprite},

        {{128,512-128,128,128},dpad_bg_sprite},

        {{293,384,50,43},dpad_up_sprite},
        {{293,472,50,40},dpad_dp_sprite},
        {{256,427,37,45},dpad_lp_sprite},
        {{343,427,38,45},dpad_rp_sprite},

        {{0,0,256,256},analog_back_sprite},

        {{128,256,96,32},trigger_sprite},
        {{0,256,96,256},trigger_back_sprite},

        {{640,256,128,64},shoulderl_sprite},
        {{768,256,128,64},shoulderl_p_sprite},

        {{384,256,128,64},shoulderr_sprite},
        {{512,256,128,64},shoulderr_p_sprite},

        {{384,0,128,128},button_a_sprite},
        {{384,128,128,128},button_x_sprite},
        {{640,0,128,128},button_b_sprite},
        {{640,128,128,128},button_y_sprite},

        {{512,0,128,128},button_ap_sprite},
        {{512,128,128,128},button_xp_sprite},
        {{768,0,128,128},button_bp_sprite},
        {{768,128,128,128},button_yp_sprite},
    };

    for(size_t i=0;i<23;i++)
        bind_sprite(sprite_map[i].key, sprite_map[i].value);

    /* Install standard event handlers */
    winhost.installEventHandler({EventHandlers::EscapeCloseWindow<BasicWindow>,nullptr,&winhost});
    winhost.installEventHandler({EventHandlers::WindowManagerCloseWindow<BasicWindow>,nullptr,&winhost});

    auto draw_element = [&](
            Sprites::Sprite const& unpressed, Sprites::Sprite const* pressed,
            Sprites::Sprite const* background,
            bool button, Vecf2 const& base, Vecf2 const& size, Vecf2 const& pos)
    {
        auto sp = &unpressed;
        if(button)
            sp = pressed;
        if(background)
            rend.drawSprite(inst,
            {
                                base.x() * size.x(),
                                base.y() * size.y()
                            },
            {size.x(), size.y()}, *background);
        rend.drawSprite(
                    inst,
        {
                        (base.x() + pos.x()) * size.x(),
                        (base.y() + pos.y()) * size.y()
                    },
        {
                        size.x(),
                        size.y()
                    }, *sp);
    };

    auto draw_axis = [&](Vecf2 base, i16 x, i16 y, bool btn, f32 scale)
    {
        draw_element(button_x_sprite, &button_xp_sprite, &analog_back_sprite,
                     btn, base,
                    {scale}, {scalar(scalar(x)/Int16_Max*128.f + 64),
                            scalar(scalar(y)/Int16_Max*128.f + 64)}
                     );
    };

    while(!winhost.closeFlag())
    {
        rend.setClearColor(inst,CRGBA(0,0,0,255));
        rend.clearBuffer(inst);

        {
            CSize win_size = winhost.windowSize();
            f32 scale_factor = .5f;

            if(win_size.w >= 960 && win_size.h >= 640)
                scale_factor = 1.f;

            /* Ensure proper scaling by using half scaling under this size */
            win_size.w = CMath::max(win_size.w, 960);
            win_size.h = CMath::max(win_size.h, 640);

            auto ctl = winhost.getControllerState(0);
            f32 axes_ruler = win_size.h - 256.f;
            draw_axis({0,axes_ruler}, ctl.axes.e.l_x,
                      ctl.axes.e.l_y, ctl.buttons.e.s_l, scale_factor);
            draw_axis({win_size.w-256.f,axes_ruler},
                      ctl.axes.e.r_x, ctl.axes.e.r_y, ctl.buttons.e.s_r,scale_factor);

            Vecf2 dpad_root = {256, axes_ruler + 128};
            draw_element(dpad_bg_sprite, nullptr, nullptr,
                         false, {dpad_root.x(), dpad_root.y()}, {scale_factor}, {});
            draw_element(empty_sprite, &dpad_up_sprite, nullptr,
                         ctl.buttons.e.p_up, dpad_root, {scale_factor}, {295-256,0});
            draw_element(empty_sprite, &dpad_dp_sprite, nullptr,
                         ctl.buttons.e.p_down, dpad_root, {scale_factor}, {295-256,472-384});
            draw_element(empty_sprite, &dpad_lp_sprite, nullptr,
                         ctl.buttons.e.p_left, dpad_root, {scale_factor}, {2,427-384});
            draw_element(empty_sprite, &dpad_rp_sprite, nullptr,
                         ctl.buttons.e.p_right, dpad_root, {scale_factor}, {345-256,427-384});

            Vecf2 back_root = {};
            draw_element(shoulderl_sprite, &shoulderl_p_sprite, nullptr,
                         ctl.buttons.e.b_l, {back_root.x(),back_root.y()}, {scale_factor}, {});
            draw_element(trigger_sprite, nullptr, &trigger_back_sprite, false,
                         {back_root.x(),back_root.y()+64}, {scale_factor},
            {0,scalar(scalar(ctl.axes.e.t_l)/Int16_Max*220.f)});

            Vecf2 right_anchor = {scalar(win_size.w-128) + back_root.x(), back_root.y()};
            draw_element(trigger_sprite, nullptr, &trigger_back_sprite, false,
                         {right_anchor.x(),right_anchor.y()+64}, {scale_factor},
                         {0,scalar(scalar(ctl.axes.e.t_r)/Int16_Max*220.f)});
            draw_element(shoulderr_sprite, &shoulderr_p_sprite, nullptr,
                         ctl.buttons.e.b_r, {right_anchor.x(),right_anchor.y()}, {scale_factor}, {});

            Vecf2 button_root = {right_anchor.x()-320.f, 0.f};
            draw_element(button_a_sprite, &button_ap_sprite, nullptr,
                         ctl.buttons.e.a, {button_root.x() + 96, button_root.y() + 196},
            {scale_factor}, {});
            draw_element(button_x_sprite, &button_xp_sprite, nullptr,
                         ctl.buttons.e.x, {button_root.x(),button_root.y() + 96},
            {scale_factor}, {});
            draw_element(button_b_sprite, &button_bp_sprite, nullptr,
                         ctl.buttons.e.b, {button_root.x() + 192,button_root.y() + 96},
            {scale_factor}, {});
            draw_element(button_y_sprite, &button_yp_sprite, nullptr,
                         ctl.buttons.e.y, {button_root.x() + 96,button_root.y()},
            {scale_factor}, {});

            Vecf2 menu_root = {win_size.w / 2.f, win_size.h - 64.f};
            draw_element(menu_sprite, &menu_p_sprite, nullptr,
                         ctl.buttons.e.back, {menu_root.x() - 96.f,menu_root.y()},
            {scale_factor}, {});
            draw_element(menu_sprite, &menu_p_sprite, nullptr,
                         ctl.buttons.e.start, {menu_root.x(),menu_root.y()},
            {scale_factor}, {});
            draw_element(menu_sprite, &menu_p_sprite, nullptr,
                         ctl.buttons.e.guide, {menu_root.x() - 48.f,menu_root.y()-64.f},
            {scale_factor}, {});
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

