#include <coffee/core/CApplication>
#include <coffee/core/CEnvironment>
#include <coffee/core/CFiles>
#include <coffee/core/CInput>
#include <coffee/core/CProfiling>
#include <coffee/core/coffee.h>
#include <coffee/core/stl_types.h>
#include <coffee/core/types/chunk.h>
#include <coffee/image/cimage.h>
#include <coffee/interfaces/cgraphics_api.h>
#include <coffee/interfaces/full_launcher.h>
#include <coffee/sdl2/CSDL2Dialog>
#include <coffee/sdl2/CSDL2SpriteWindow>
#include <coffee/sdl2/CSDL2System>
#include <coffee/strings/info.h>
#include <coffee/strings/libc_types.h>
#include <coffee/strings/url_types.h>
#include <peripherals/stl/time_types.h>

#include <coffee/core/CDebug>

using namespace Coffee;
using namespace Display;

using BasicWindow = SDL2WindowHost;
using Sprites     = SDL2SpriteRenderer;

const constexpr szptr num_points = 10;

static PtF             sprite_pos[num_points] = {};
static size_2d<scalar> sprite_scale           = {1, 1};
static Size            window_size            = {};

void WindowResize_1(void*, const Event& e, c_cptr d)
{
    if(e.type == Event::Resize)
    {
        auto ev     = (ResizeEvent const*)d;
        window_size = *ev;
    }
}

void TouchInput_1(void*, const Input::CIEvent& e, c_cptr d)
{
    using namespace Input;

    if(e.type == CIEvent::MultiTouch)
    {
        const CIMTouchMotionEvent& tch = *(const CIMTouchMotionEvent*)d;
        if(tch.fingers == 2)
        {
            scalar v = tch.dist;
            v /= Int16_Max;
            v += 1.f;
            size_2d<scalar> s = {v, v};
            sprite_scale.w *= s.w;
            sprite_scale.h *= s.h;
        }
    } else if(e.type == CIEvent::TouchMotion)
    {
        const CITouchMotionEvent& tch = *(const CITouchMotionEvent*)d;
        if(tch.finger > num_points)
            return;
        if(!tch.hover)
        {
            sprite_pos[tch.finger] = tch.origin;
            sprite_pos[tch.finger].x *= window_size.w;
            sprite_pos[tch.finger].y *= window_size.h;
            sprite_pos[tch.finger].x -= 64;
            sprite_pos[tch.finger].y -= 64;
        }
    } else if(e.type == CIEvent::MouseMove)
    {
        const CIMouseMoveEvent& mbn = *(const CIMouseMoveEvent*)d;
        if(mbn.btn != 0)
        {
            sprite_pos[0] = mbn.origin;
            sprite_pos[0].x -= 64;
            sprite_pos[0].y -= 64;
        }
    } else if(e.type == CIEvent::TouchTap)
    {
        const CITouchTapEvent& tch = *(const CITouchTapEvent*)d;
        if(tch.finger > num_points)
            return;
        if(tch.pressed)
        {
            sprite_pos[tch.finger] = tch.pos;
            sprite_pos[tch.finger].x *= window_size.w;
            sprite_pos[tch.finger].y *= window_size.h;
            sprite_pos[tch.finger].x -= 64;
            sprite_pos[tch.finger].y -= 64;
        }
    } else if(e.type == CIEvent::MouseButton)
    {
        const CIMouseButtonEvent& mbn = *(const CIMouseButtonEvent*)d;
        if(mbn.mod & CIMouseButtonEvent::Pressed)
        {
            sprite_pos[0] = mbn.pos;
            sprite_pos[0].x -= 64;
            sprite_pos[0].y -= 64;
        }
    }
}

void ExitHandler_1(void* ptr, const Input::CIEvent& e, c_cptr d)
{
    using namespace Input;

    SDL2WindowHost* host = (SDL2WindowHost*)ptr;

    if(e.type == CIEvent::QuitSign)
    {
        host->closeWindow();
    } else if(e.type == CIEvent::Keyboard)
    {
        CIKeyEvent const* kev = (CIKeyEvent const*)d;
        if(kev->key == CK_Escape)
            host->closeWindow();
    }
}

struct RenderData
{
    Sprites           rend;
    Sprites::Renderer r;
    Sprites::Texture  t;
    Sprites::Sprite   sprite;
    rgba_t            clearCol;
};

i32 coffee_main(i32, cstring_w*)
{
    C_UNUSED(SubsystemWrapper<SDL2::SDL2> sys1);

    /* Set file prefix, basically a cwd but only for resources */
    cDebug("Current directory: {0}", Env::CurrentDir());

    auto setup = [](BasicWindow& test, RenderData* data) {
        /* Create a sprite renderer context */
        data->rend = Sprites(&test);

        CString err;
        if(!data->rend.init(&err))
        {
            cDebug("Initialization error: {0}", err);
            return;
        }

        /* Create a renderer handle */
        data->r = data->rend.createRenderer();
        Profiler::Profile("Context handle");

        /* Create a texture */
        if(data->rend.createTexture(
               data->r,
               1,
               &data->t,
               PixFmt::RGBA8UI,
               RSCA::Streaming,
               Size(128, 128)))
        {
            Profiler::Profile("Texture creation");

            /* Map a texture into memory */
            auto texfile = "particle_sprite.png"_rsc;

            /* Decode file to RGBA data */
            stb::image_rw img = {};
            stb::LoadData(&img, texfile, PixCmp::RGBA);

            /* Copy texture into texture memory */
            {
                rgba_t* pdata =
                    C_FCAST<rgba_t*>(data->rend.mapTexture(data->t));
                MemCpy(
                    C_OCAST<Bytes>(img),
                    Bytes::From(pdata, img.size.area() * img.bpp));
                data->rend.unmapTexture(data->t);
            }

            Profiler::Profile("Texture load");
        }

        /* Create a sprite from the texture */
        Rect src(0, 0, 128, 128);
        data->rend.createSprite(data->t, src, &data->sprite);

        /* Show the window */
        test.showWindow();

        data->clearCol = rgba_t(127, 0, 0);

        /* Set clear color for buffer */
        data->rend.setClearColor(data->r, data->clearCol);

        cDebug("Resolution: {0}", test.windowSize());
    };

    auto loop = [](BasicWindow& test, RenderData* data) {
        data->rend.setClearColor(data->r, data->clearCol);

        data->rend.clearBuffer(data->r);
        for(szptr i = 0; i < num_points; i++)
            data->rend.drawSprite(
                data->r, sprite_pos[i], sprite_scale, data->sprite);

        data->clearCol.r = (Time<>::CurrentTimestamp() * 1000) % 255;

        data->rend.swapBuffers(data->r);
        test.pollEvents();
    };

    auto cleanup = [](BasicWindow& test, RenderData* data) {
        test.hideWindow();

        data->rend.destroyTexture(1, &data->t);
        data->rend.destroyRenderer(data->r);

        data->rend.spritesTerminate();
    };

    /* Create a window host for the renderer */
    auto visual = GetDefaultVisual(3, 3);

    using ELD = EventLoopData<BasicWindow, RenderData>;

    ELD* eventData = new ELD{MkUq<BasicWindow>(),
                             MkUq<RenderData>(),
                             setup,
                             loop,
                             cleanup,
                             std::move(visual)};

    eventData->renderer->installEventHandler(
        {TouchInput_1, nullptr, eventData->renderer.get()});
    eventData->renderer->installEventHandler(
        {ExitHandler_1, nullptr, eventData->renderer.get()});
    eventData->renderer->installEventHandler(
        {WindowResize_1, nullptr, eventData->renderer.get()});

    CString err;
    BasicWindow::execEventLoop(*eventData, visual, err);

    return 0;
}

COFFEE_APPLICATION_MAIN(coffee_main)
