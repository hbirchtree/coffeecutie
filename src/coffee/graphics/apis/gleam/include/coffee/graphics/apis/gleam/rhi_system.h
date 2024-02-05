#pragma once

#include <coffee/comp_app/services.h>
#include <coffee/components/subsystem.h>
#include <coffee/core/debug/formatting.h>

#include "rhi.h"
#include "rhi_context.h"

#include <coffee/core/types/display/event.h>

namespace gleam {

using system_manifest = compo::SubsystemManifest<
    type_safety::empty_list_t,
    type_safety::empty_list_t,
    type_safety::type_list_t<
        comp_app::BasicEventBus<Coffee::Display::Event>,
        comp_app::GraphicsFramebuffer>>;

class system : public compo::RestrictedSubsystem<system, system_manifest>,
               public gleam::api,
               public comp_app::AppLoadableService
{
    using reset_status = gleam::context::api::reset_status;

  public:
    using type = system;
    using gleam::api::load;

    system() : context_checker{context_robustness()}
    {
        priority = 900;
    }

    void load_context_checker()
    {
        context_notifies = context_checker.reset_strategy()
                           == gleam::context::api::strategy::lose_on_reset;
    }

    void start_restricted(Proxy& e, time_point const& ts)
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
    void end_restricted(Proxy&, time_point const&)
    {
    }

    gleam::context::api& context_checker;
    std::optional<bool>  context_notifies;

  protected:
    bool m_viewport_not_set{false};

    void load(entity_container& e, comp_app::app_error&)
    {
        using Coffee::Display::Event;
        using Coffee::Display::ResizeEvent;

        auto display_bus = e.service<comp_app::BasicEventBus<Event>>();

        if(!display_bus)
            return;

        display_bus->addEventFunction<ResizeEvent>(
            0, [this](Event&, ResizeEvent* resize) {
                if constexpr(compile_info::platform::is_emscripten)
                    return;
                auto new_size = resize->convert<i32>();
                if(new_size.w == 0)
                    m_viewport_not_set = true;
                default_rendertarget()->resize({0, 0, new_size.w, new_size.h});
            });
    }

    void activate_resize(Proxy& e)
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
            m_viewport_not_set = false;
            m_last_size        = size;
        }
    }

    void check_context()
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

    comp_app::size_2d_t m_last_size;
    time_point          m_next_stats{};
};

} // namespace gleam
