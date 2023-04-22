#include <coffee/core/CApplication>
#include <coffee/core/CFiles>
#include <coffee/core/CInput>
#include <coffee/core/coffee.h>
#include <coffee/core/platform_data.h>
#include <coffee/core/types/debug/severity.h>
#include <coffee/graphics/apis/gleam/rhi.h>
#include <coffee/interfaces/full_launcher.h>
#include <coffee/strings/info.h>
#include <coffee/strings/libc_types.h>
#include <coffee/windowing/renderer/renderer.h>
#include <peripherals/stl/time_types.h>

#include <coffee/core/CDebug>

#if defined(FEATURE_ENABLE_ASIO)
#include <coffee/asio/net_profiling.h>
#endif

using namespace Coffee;
using namespace Display;

using CDRenderer = CSDL2Renderer;

struct SharedData
{
    u16        frame_count{0};
    Timestamp  frame_ts{0};
    gleam::api api;

    std::shared_ptr<gleam::texture_2d_t>   tex;
    std::shared_ptr<gleam::texture_2d_t>   depth_tex;
    std::shared_ptr<gleam::rendertarget_t> offscreen;
};

void setup_fun(CDRenderer& renderer, SharedData* data)
{
    renderer.setWindowTitle("GL extensions");

    cDebug("GL extensions:");
    for(auto ext : gleam::api::query_native_extensions())
        cBasicPrintNoNL("{0} ", ext);
    cBasicPrint("");
    cDebug(
        "Framebuffer size: {0}, window size: {1}",
        renderer.framebufferSize(),
        renderer.windowSize());
    cDebug("Framebuffer scale: {0}", platform::info::device::display::dpi());
    cDebug("Monitor: {0}", renderer.monitor());

    data->api.load({
//        .api_version    = 0x330,
//        .api_type       = gleam::api_type_t::core,
//        .api_extensions = std::make_optional<gleam::api::extensions_set>({}),
    });
    data->api.debug().enable();
    data->api.debug().add_callback(
        [](gleam::group::debug_severity, std::string_view const& message) {
            cDebug("GL debug message: {0}", message);
        });

    auto _ = data->api.debug().scope(__PRETTY_FUNCTION__);

    data->tex = data->api.alloc_texture(
        gleam::textures::d2, typing::pixels::PixDesc(PixFmt::RGB8), 2);
    data->tex->alloc({1280, 720, 1});
    gleam::cmd::bind_texture(
        gleam::group::texture_target::texture_2d, data->tex->m_handle);
    data->depth_tex = data->api.alloc_texture(
        gleam::textures::d2, typing::pixels::PixDesc(PixFmt::Depth32F), 1);
    data->depth_tex->alloc({1280, 720, 1});

    data->offscreen = data->api.alloc_rendertarget();
    data->offscreen->alloc();
    data->offscreen->attach(
        gleam::render_targets::attachment::color, *data->tex, 0);
    data->offscreen->attach(
        gleam::render_targets::attachment::depth, *data->depth_tex, 0);

    gleam::cmd::check_named_framebuffer_status(
        data->offscreen->m_handle,
        gleam::group::framebuffer_target::read_framebuffer);

    {
        auto [vendor, device] = data->api.device();
        auto driver = data->api.device_driver().value_or("Unknown driver");
        cDebug("Device: device={0} {1} driver={2}", vendor, device, driver);
        auto [major, minor]   = data->api.api_version();
        auto [nmajor, nminor] = data->api.query_native_version();
        cDebug(
            "Version: api={2} ver={0}.{1}", major, minor, data->api.api_name());
        cDebug("Native version: ver={0}.{1}", nmajor, nminor);
        auto [slmajor, slminor] = data->api.shaderlang_version();
        auto slname             = data->api.shaderlang_name();
        cDebug(
            "Shading language: lang={2} ver={0}.{1}", slmajor, slminor, slname);
    }

    renderer.popErrorMessage(debug::Severity::Information, "Hello!", "Goodbye");
    renderer.showWindow();
}

void loop_fun(CDRenderer& renderer, SharedData* data)
{
    data->api.default_rendertarget()->clear(Vecf4(1, 1, 0, 1));

    data->offscreen->copy(
        *data->api.default_rendertarget(),
        {0, 0, 1280, 720},
        {0, 0},
        gleam::render_targets::attachment::back,
        gleam::render_targets::attachment::color);

    if(data->frame_ts <= Time<>::CurrentTimestamp())
    {
        cDebug("FPS: {0}", data->frame_count);
        data->frame_ts    = Time<>::CurrentTimestamp() + 1000;
        data->frame_count = 0;
    }
    data->frame_count++;

    renderer.pollEvents();
    renderer.swapBuffers();
}

void cleanup_fun(CDRenderer&, SharedData* data)
{
    data->tex->dealloc();
    data->offscreen->dealloc();
}

i32 coffee_main(i32, cstring_w*)
{
    RuntimeQueue::CreateNewQueue("Main");

#if defined(FEATURE_ENABLE_ASIO)
    C_UNUSED(auto _) = Net::RegisterProfiling();
    cDebug("Installing net profiler");
#endif

    std::string err;

    return AutoExec<RHI::GLEAM::GLEAM_API, CDRenderer, SharedData>(
        [](std::shared_ptr<CDRenderer> r,
           std::shared_ptr<SharedData>,
           Display::Properties& visual) {
            visual.flags ^= Properties::Windowed;
            visual.flags |= Properties::WindowedFullScreen;

            using namespace Input;
            using namespace EventHandlers;

            //            r->installEventHandler(
            //                EHandle<Event>::MkHandler(WindowResize<GLEAMAPI>()));
            r->installEventHandler(EHandle<CIEvent>::MkHandler(
                ExitOn<OnKey<Input::CK_Escape>>(r->window())));
            r->installEventHandler(
                EHandle<CIEvent>::MkHandler(ExitOn<OnQuit>(r->window())));
            r->installEventHandler(EHandle<CIEvent>::MkHandler(
                FullscreenOn<AnyIKey<
                    KeyCombo<
                        CK_EnterNL,
                        CIKeyEvent::KeyModifiers::RAltModifier>,
                    KeyCombo<CK_F11>>>(r->window())));
        },
        setup_fun,
        loop_fun,
        cleanup_fun);
}

COFFEE_APPLICATION_MAIN(coffee_main)
