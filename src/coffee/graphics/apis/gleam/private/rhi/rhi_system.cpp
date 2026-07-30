#include "peripherals/concepts/graphics_api.h"
#include <chrono>
#include <coffee/graphics/apis/gleam/rhi_system.h>

#include <coffee/comp_app/gl_config.h>
#include <coffee/comp_app/services.h>
#include <coffee/comp_app/subsystems.h>
#include <coffee/components/types.h>
#include <coffee/graphics/apis/gleam/rhi_versioning.h>
#include <glw/enums/FramebufferTarget.h>
#include <glw/enums/GetPName.h>
#include <glw/enums/RenderbufferTarget.h>
#include <peripherals/constants.h>
#include <peripherals/semantic/chunk.h>
#include <peripherals/typing/enum/pixels/format_transform.h>
#include <peripherals/typing/geometry/size.h>
#include <platforms/profiling.h>

#include <coffee/graphics/apis/gleam/rhi_query.h>

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
    if(compile_info::debug_mode && api_type() == api_type_t::es &&
       api_version() == std::make_tuple<u32, u32>(2, 0))
    {
        Coffee::DProfContext _("gleam::system::Setting up capture FBO");
        m_capture_fbo_active = true;
        m_screenshot_provider =
            container.service<comp_app::ScreenshotProvider>();
        m_color_capture =
            alloc_texture(textures::d2, PixDesc(comp_app::pix_fmt::RGBA8), 1);
        cmd::gen_renderbuffers(semantic::SpanOne(m_depth_capture.hnd));
        m_color_capture->alloc(size_3d<u32>{16u, 16u, 1u});
        m_capture_fbo = alloc_rendertarget();
        m_capture_fbo->alloc();
        m_capture_fbo->attach(
            render_targets::attachment::color, *m_color_capture, 0);
        i32 currentFbo{};
        cmd::get_integerv(
            group::get_prop::framebuffer_binding, SpanOne(currentFbo));
        cmd::bind_framebuffer(
            group::framebuffer_target::framebuffer, m_capture_fbo->m_handle);
        cmd::framebuffer_renderbuffer(
            group::framebuffer_target::framebuffer,
            group::framebuffer_attachment::depth_attachment,
            group::renderbuffer_target::renderbuffer,
            m_depth_capture);
        cmd::bind_framebuffer(
            group::framebuffer_target::framebuffer, currentFbo);
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
            "draws: "
            "{} submits, {} draws, {} instances, {} failed draws, "
            "{} triangles, {} triangle strips, {} other prims, "
            "{} async compiles {} failed async compiles, "
            "binds: {} render targets, {} programs, {} vaos; "
            "buffers: "
            "{} transfers, {} bytes transferred, "
            "{} mappings, {} bytes mapped; "
            "textures: {} allocations, {} uploads, {} bytes transferred; ",
            usage.draw.submits,
            usage.draw.draws,
            usage.draw.instances,
            usage.draw.failed_draws,
            usage.draw.triangles,
            usage.draw.triangle_strips,
            usage.draw.other_prims,
            // async compiles
            usage.draw.async_compiles,
            usage.draw.failed_async_compiles,
            // binds
            usage.draw.framebuffers_bound,
            usage.draw.programs_bound,
            usage.draw.vaos_bound,
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
        m_next_stats = ts + 5s;
    }
    this->usage() = {};

    std::vector<std::string> finished_timers;
    for(auto& [name, timer] : m_tracked_timings)
    {
        if(auto time = timer.timer->result())
        {
            finalize_timer(std::move(timer));
            finished_timers.push_back(name);
        }
    }
    for(auto const& timer : finished_timers)
        m_tracked_timings.erase(timer);
}

void system::end_restricted(Proxy&, time_point const&)
{
    if(m_screenshot_provider && m_capture_fbo_active &&
       m_screenshot_provider->captureRequested())
    {
        if(!m_capture_requested)
        {
            Coffee::DProfContext _(
                "gleam::system::end_restricted::Enabling capture FBO");
            m_capture_fbo->internal_bind(
                group::framebuffer_target::framebuffer);
            default_rendertarget()->m_handle = m_capture_fbo->m_handle.hnd;
            m_capture_requested              = true;
        } else
        {
            Coffee::DProfContext _(
                "gleam::system::end_restricted::Submitting capture FBO");
            m_screenshot_provider->signalCaptureReady(
                m_capture_fbo->m_handle.hnd);
            default_rendertarget()->m_handle = 0;
            m_capture_requested              = false;
        }
    }
}

void system::activate_resize(Proxy& e)
{
    Coffee::DProfContext _;
    //        if constexpr(compile_info::platform::is_emscripten)
    //            return;

    if(auto fb = e.service<comp_app::GraphicsFramebuffer>())
    {
        // TODO: Find out why this is so expensive on Lima driver
        auto size = fb->size();
        if(size.w == 0)
            return;
        if(size.w == m_last_size.w && size.h == m_last_size.h)
            return;
        default_rendertarget()->resize({0, 0, size.w, size.h});
        if(m_capture_fbo_active)
        {
            m_color_capture->alloc(
                size_3d<u32>{
                    static_cast<u32>(size.w), static_cast<u32>(size.h), 1u});
            cmd::bind_renderbuffer(
                group::renderbuffer_target::renderbuffer, m_depth_capture);
            cmd::renderbuffer_storage(
                group::renderbuffer_target::renderbuffer,
                group::internal_format::depth_component16,
                size);
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

void system::track_timer(std::shared_ptr<query_t>&& timer, std::string const& name)
{
    if(m_tracked_timings.contains(name))
        return;
    auto& tracker = m_tracked_timings[name];
    tracker = {
        .timer = std::move(timer),
        .name = name,
        .start_time = compo::clock::now(),
    };
}

bool system::is_timer_pending(std::string const& name)
{
    return m_tracked_timings.contains(name);
}

void system::finalize_timer(system::timing_t&& timer)
{
    using namespace platform::profiling;

    auto props = RuntimeProperties::get_properties();
    auto start = timer.start_time.time_since_epoch();
    props.push(*props.context, datapoint_t{
        .tid = 0x8005,
        .name = timer.name,
        .component = COFFEE_COMPONENT_NAME,
        .thread_name = "GPU",
        .ts = start,
        .flags = {
            .type = datapoint_t::push,
        },
    });
    props.push(*props.context, datapoint_t{
        .tid = 0x8005,
        .name = timer.name,
        .component = COFFEE_COMPONENT_NAME,
        .thread_name = "GPU",
        .ts = start + std::chrono::nanoseconds(timer.timer->resultSync()),
        .flags = {
            .type = datapoint_t::pop,
        },
    });
    timer.timer->dealloc();
}

system::gpu_timer_t::gpu_timer_t(gleam::system* system, std::string const& name)
{
    if(system->is_timer_pending(name))
        return;
    m_system = system;
    m_timer = system->alloc_query(queries::time);
    m_name = name;
    m_timer->alloc();
    m_timer->start();
}

system::gpu_timer_t::~gpu_timer_t()
{
    if(!m_timer || !m_system)
        return;
    m_timer->stop();
    m_system->track_timer(std::move(m_timer), m_name);
}

} // namespace gleam
