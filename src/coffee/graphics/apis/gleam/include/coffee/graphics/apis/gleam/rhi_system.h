#pragma once

#include <coffee/comp_app/services.h>
#include <coffee/components/subsystem.h>
#include <coffee/core/debug/formatting.h>

#include "coffee/graphics/apis/gleam/rhi_query.h"
#include "coffee/graphics/apis/gleam/rhi_rendertarget.h"
#include "coffee/graphics/apis/gleam/rhi_texture.h"
#include "coffee/graphics/apis/gleam/rhi_versioning.h"
#include "rhi.h"
#include "rhi_context.h"
#include "types.h"

#include <coffee/core/types/display/event.h>

namespace gleam {

using system_manifest = compo::SubsystemManifest<
    type_safety::empty_list_t,
    type_safety::empty_list_t,
    type_safety::type_list_t<
        comp_app::BasicEventBus<Coffee::Display::Event>,
        comp_app::GraphicsFramebuffer>>;

class system
    : public compo::RestrictedSubsystem<system, system_manifest>
    , public gleam::api
    , public comp_app::AppLoadableService
{
    using reset_status = gleam::context::api::reset_status;

  public:
    using type = system;

    virtual bool main_thread_only() const override
    {
        return true;
    }

    system()
        : context_checker{context_robustness()}
    {
        priority = 900;
    }

    optional<error> load(
        compo::EntityContainer& container,
        load_options_t          options = default_options);

    void load_context_checker()
    {
        context_notifies = context_checker.reset_strategy() ==
                           gleam::context::api::strategy::lose_on_reset;
    }

    void start_restricted(Proxy& e, time_point const& ts);

    void end_restricted(Proxy&, time_point const&);

    /*!
     * Take start()/stop()'d TIME_ELAPSED query and see it to completion
     */
    void track_timer(std::shared_ptr<query_t>&& timer, std::string const& name);
    bool is_timer_pending(std::string const& name);

    struct gpu_timer_t
    {
        gpu_timer_t(system* system, std::string const& name);
        ~gpu_timer_t();

        system* m_system{nullptr};
        std::shared_ptr<query_t> m_timer;
        std::string m_name;
    };

    gpu_timer_t gpu_timer(std::string const& name)
    {
        return gpu_timer_t(this, name);
    }

    gleam::context::api& context_checker;
    std::optional<bool>  context_notifies;

  protected:
    bool m_viewport_not_set{false};

    virtual void load(entity_container& e, comp_app::app_error&) final;

    void activate_resize(Proxy& e);

    void check_context();

    comp_app::size_2d_t m_last_size;
    bool                m_resize_dirty{true};
    time_point          m_next_resize_poll{};
    time_point          m_next_stats{};

    struct timing_t
    {
        std::shared_ptr<query_t> timer;
        std::string              name;
        compo::time_point        start_time;
    };

    void finalize_timer(timing_t&& timer);

    // Capture FBO for GL ES 2.0 platforms
    // Because ReadPixels doesn't work well
    comp_app::interfaces::ScreenshotProvider* m_screenshot_provider{};
    std::shared_ptr<rendertarget_t>           m_capture_fbo{};
    std::shared_ptr<texture_t>                m_color_capture{};
    std::map<std::string, timing_t>           m_tracked_timings;
    hnd                                       m_depth_capture{};
    bool                                      m_capture_fbo_active{false};
    bool                                      m_capture_requested{false};
};

} // namespace gleam
