#include "coffee/comp_app/services.h"
#include "coffee/graphics/apis/gleam/rhi_versioning.h"
#include "glw/enums/FramebufferTarget.h"
#include "glw/enums/GetPName.h"
#include "glw/enums/RenderbufferTarget.h"
#include "peripherals/semantic/chunk.h"
#include "peripherals/typing/enum/pixels/format_transform.h"
#include "peripherals/typing/geometry/size.h"
#include <coffee/graphics/apis/gleam/rhi_system.h>

#include <coffee/comp_app/gl_config.h>
#include <coffee/comp_app/subsystems.h>

#if defined(FEATURE_ENABLE_ComponentBundleSetup_DummyPlug)
#include <coffee/comp_app/dummy_plug.h>
#endif

namespace gleam {

optional<error> system::load(
    compo::EntityContainer& container, load_options_t options)
{
#if defined(FEATURE_ENABLE_ComponentBundleSetup_DummyPlug)
    auto loader = container.service<comp_app::AppLoader>();
    if(auto const& dummy_plug = loader->config<comp_app::dummy_plug::Config>();
       dummy_plug.enabled)
    {
        auto const& gl_config  = loader->config<comp_app::GLConfig>();
        auto const& gfx_config = dummy_plug.graphics_config;
        options.api_type = gfx_config.value("profile", std::string()) == "Core"
                               ? api_type_t::core
                               : api_type_t::es;
        options.api_version = version_tuple_to_u32(
            std::make_tuple(
                gfx_config.value("major", 0u), gfx_config.value("minor", 0u)));
    }
#endif
    auto out = api::load(options);
    if(api_type() == api_type_t::es && api_version() == std::make_tuple<u32, u32>(2, 0))
    {
        m_capture_fbo_active = true;
        m_screenshot_provider = container.service<comp_app::ScreenshotProvider>();
        m_color_capture = alloc_texture(textures::d2, PixDesc(comp_app::pix_fmt::RGBA8), 1);
        cmd::gen_renderbuffers(semantic::SpanOne(m_depth_capture.hnd));
        m_color_capture->alloc(size_3d<u32>(16, 16, 1));
        m_capture_fbo = alloc_rendertarget();
        m_capture_fbo->alloc();
        m_capture_fbo->attach(render_targets::attachment::color, *m_color_capture, 0);
        i32 currentFbo{};
        cmd::get_integerv(group::get_prop::framebuffer_binding, SpanOne(currentFbo));
        cmd::bind_framebuffer(group::framebuffer_target::framebuffer, m_capture_fbo->m_handle);
        cmd::framebuffer_renderbuffer(
            group::framebuffer_target::framebuffer,
            group::framebuffer_attachment::depth_attachment,
            group::renderbuffer_target::renderbuffer, m_depth_capture);
        cmd::bind_framebuffer(group::framebuffer_target::framebuffer, currentFbo);
    }
    return out;
}

void system::start_restricted(Proxy& e, time_point const& ts)
{
    using namespace std::chrono_literals;
    using namespace Coffee::Logging;

    activate_resize(e);
    check_context();

    if(m_next_stats < ts)
    {
        gleam::usage const& usage = this->usage();
        cDebug(
            "gleam operations: "
            "draws: {} draws, {} instances, {} failed draws, "
            "{} triangles, {} triangle strips, {} other prims; "
            "buffers: {} transfers, {} bytes transferred, "
            "{} mappings, {} bytes mapped; "
            "textures: {} allocations, {} uploads, {} bytes transferred; ",
            usage.draw.draws,
            usage.draw.instances,
            usage.draw.failed_draws,
            usage.draw.triangles,
            usage.draw.triangle_strips,
            usage.draw.other_prims,
            // buffer info
            usage.buffers.uploads,
            usage.buffers.upload_data,
            usage.buffers.mappings,
            usage.buffers.mapped_data,
            // texture info
            0,
            usage.texture.texture_uploads,
            0
            //
        );
        m_next_stats = ts + 1s;
    }
    this->usage() = {};
}

void system::end_restricted(Proxy&, time_point const&)
{
    if(m_screenshot_provider && m_capture_fbo_active && m_screenshot_provider->captureRequested())
    {
        if(!m_capture_requested)
        {
            m_capture_fbo->internal_bind(group::framebuffer_target::framebuffer);
            default_rendertarget()->m_handle = m_capture_fbo->m_handle.hnd;
            m_capture_requested = true;
        } else
        {
            m_screenshot_provider->signalCaptureReady(m_capture_fbo->m_handle.hnd);
            default_rendertarget()->m_handle = 0;
            m_capture_requested = false;
        }
    }
}

void system::activate_resize(Proxy& e)
{
    //        if constexpr(compile_info::platform::is_emscripten)
    //            return;

    if(auto fb = e.service<comp_app::GraphicsFramebuffer>())
    {
        auto size = fb->size();
        if(size.w == 0)
            return;
        if(size.w == m_last_size.w && size.h == m_last_size.h)
            return;
        default_rendertarget()->resize({0, 0, size.w, size.h});
        if(m_capture_fbo_active)
        {
            m_color_capture->alloc(size_3d<u32>(size.w, size.h, 1));
            cmd::bind_renderbuffer(group::renderbuffer_target::renderbuffer, m_depth_capture);
            cmd::renderbuffer_storage(group::renderbuffer_target::renderbuffer, group::internal_format::depth24_stencil8, size);
            cmd::bind_renderbuffer(group::renderbuffer_target::renderbuffer, 0);
        }
        m_viewport_not_set = false;
        m_last_size        = size;
    }
}

void system::check_context()
{
    if(!context_checker.reset_state_supported())
        return;
    if(!context_notifies.has_value() || *context_notifies)
        return;
    auto lost_state = context_checker.context_state();
    if(lost_state == reset_status::no_error)
        return;

    //        auto state_string = magic_enum::enum_name(lost_state);
    // TODO: Trigger a reload
    Throw(undefined_behavior("context lost"));
    //            std::string(state_string.begin(), state_string.end())));
}

void system::load(entity_container& e, comp_app::app_error&)
{
    using Coffee::Display::Event;
    using Coffee::Display::ResizeEvent;

    auto display_bus = e.service<comp_app::BasicEventBus<Event>>();

    if(!display_bus)
        return;

    display_bus->addEventFunction<ResizeEvent>(
        0, [this](Event&, ResizeEvent* resize) {
            // if constexpr(compile_info::platform::is_emscripten)
            //     return;
            auto new_size = resize->convert<i32>();
            if(new_size.w == 0)
                m_viewport_not_set = true;
            default_rendertarget()->resize({0, 0, new_size.w, new_size.h});
        });
}
} // namespace gleam
