#pragma once

#include <coffee/comp_app/services.h>
#include <coffee/components/subsystem.h>
#include <coffee/core/debug/formatting.h>

#include "coffee/graphics/apis/gleam/rhi_rendertarget.h"
#include "coffee/graphics/apis/gleam/rhi_texture.h"
#include "coffee/graphics/apis/gleam/rhi_versioning.h"
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

class system
    : public compo::RestrictedSubsystem<system, system_manifest>
    , public gleam::api
    , public comp_app::AppLoadableService
{
    using reset_status = gleam::context::api::reset_status;

  public:
    using type = system;

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

    gleam::context::api& context_checker;
    std::optional<bool>  context_notifies;

  protected:
    bool m_viewport_not_set{false};

    virtual void load(entity_container& e, comp_app::app_error&) final;

    void activate_resize(Proxy& e);

    void check_context();

    comp_app::size_2d_t m_last_size;
    time_point          m_next_stats{};

    // Capture FBO for GL ES 2.0 platforms
    // Because ReadPixels doesn't work well
    comp_app::interfaces::ScreenshotProvider* m_screenshot_provider{};
    std::shared_ptr<rendertarget_t>           m_capture_fbo{};
    std::shared_ptr<texture_t>                m_color_capture{};
    hnd                                       m_depth_capture{};
    bool                                      m_capture_fbo_active{false};
    bool                                      m_capture_requested{false};
};

} // namespace gleam
