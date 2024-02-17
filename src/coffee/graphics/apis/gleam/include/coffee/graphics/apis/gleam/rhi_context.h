#pragma once

#include "rhi_features.h"
#include "rhi_translate.h"

#include <glw/extensions/KHR_robustness.h>

namespace gleam::context {

#if(GLEAM_MAX_VERSION >= 0x450 || GLEAM_MAX_VERSION_ES >= 0x320) && \
    defined(GL_KHR_robustness)
struct api
{
    enum class strategy
    {
        lose_on_reset,
        no_notification,
    };

    using reset_status = gl::group::graphics_reset_status;

    constexpr bool reset_state_supported() const
    {
        return true;
    }

    auto reset_strategy() const
    {
        i32 strategy{0};
        cmd::get_integerv(
            static_cast<group::get_prop>(
                gl::khr::robustness::values::reset_notification_strategy),
            SpanOne(strategy));
        if(strategy == gl::khr::robustness::values::no_reset_notification)
            return strategy::no_notification;
        else
            return strategy::lose_on_reset;
    }

    auto context_state() const
    {
        return static_cast<reset_status>(cmd::get_graphics_reset_status());
    }
};
#else
struct api
{
    enum class strategy
    {
        lose_on_reset,
        no_notification,
    };
    using reset_status = gl::group::graphics_reset_status;

    constexpr bool reset_state_supported() const
    {
        return false;
    }

    auto reset_strategy() const
    {
        return strategy::no_notification;
    }

    auto context_state() const
    {
        return reset_status::no_error;
    }
};
#endif

} // namespace gleam::context
