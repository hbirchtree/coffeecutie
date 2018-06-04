#include <coffee/core/CApplication>
#include <coffee/core/CFiles>
#include <coffee/core/CInput>
#include <coffee/core/CProfiling>
#include <coffee/core/types/cdef/memsafe.h>

#include <coffee/CImage>

#include <coffee/sdl2/CSDL2Dialog>
#include <coffee/sdl2/CSDL2SpriteWindow>
#include <coffee/sdl2/CSDL2System>

#include <coffee/core/CDebug>

using namespace Coffee;
using namespace Display;

using BasicWindow = SDL2WindowHost;
using Sprites     = SDL2SpriteRenderer;

const constexpr szptr num_points = 10;

CPointF sprite_pos[num_points] = {};
CSizeF  sprite_scale           = {1, 1};
CSize   window_size            = {};

bool exit_flag = false;

void WindowResize_1(void*, const CDEvent& e, c_cptr d)
{
    if(e.type == CDEvent::Resize)
    {
        auto ev     = (CDResizeEvent const*)d;
        window_size = *ev;
    }
}

void TouchInput_1(void*, const CIEvent& e, c_cptr d)
{
    if(e.type == CIEvent::MultiTouch)
    {
        const CIMTouchMotionEvent& tch = *(const CIMTouchMotionEvent*)d;
        if(tch.fingers == 2)
        {
            scalar v = tch.dist;
            v /= Int16_Max;
            v += 1.f;
            CSizeF s = {v, v};
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

void ExitHandler_1(void* ptr, const CIEvent& e, c_cptr d)
{
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
    CRGBA             clearCol;
};

int32 coffee_main(int32, cstring_w*)
{
    SubsystemWrapper<SDL2::SDL2> sys1;
    C_UNUSED(sys1);

    /* Set file prefix, basically a cwd but only for resources */
    CResources::FileResourcePrefix("sample_data/ctest_hud/");
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
               PixelFormat::RGBA8UI,
               ResourceAccess::Streaming,
               CSize(128, 128)))
        {
            Profiler::Profile("Texture creation");

            /* Map a texture into memory */
            auto texfile = "particle_sprite.png"_rsc;

            /* Decode file to RGBA data */
            stb::image_rw img = {};
            stb::LoadData(&img, texfile, PixCmp::RGBA);

            /* Copy texture into texture memory */
            {
                CRGBA* pdata = C_FCAST<CRGBA*>(data->rend.mapTexture(data->t));
                MemCpy(
                    C_OCAST<Bytes>(img),
                    Bytes::From(pdata, img.size.area() * img.bpp));
                data->rend.unmapTexture(data->t);
            }

            /* Clean up */
            stb::ImageFree(&img);
            CResources::FileUnmap(texfile);
            Profiler::Profile("Texture load");
        }

        /* Create a sprite from the texture */
        CRect src(0, 0, 128, 128);
        data->rend.createSprite(data->t, src, &data->sprite);

        /* Show the window */
        test.showWindow();

        data->clearCol = CRGBA(255, 0, 0);

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

        data->clearCol.r = (Time::CurrentTimestamp() * 1000) % 255;

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
    auto visual = GetDefaultVisual(2, 0);

    using ELD = EventLoopData<BasicWindow, RenderData>;

    ELD* eventData = new ELD{
        MkUq<BasicWindow>(), MkUq<RenderData>(), setup, loop, cleanup, 0, {}};

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
