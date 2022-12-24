#pragma once

#include <coffee/components/subsystem.h>

#include "rhi.h"

namespace gleam {

using system_manifest = Coffee::Components::SubsystemManifest<
    type_safety::empty_list_t,
    type_safety::empty_list_t,
    type_safety::empty_list_t>;

class system
    : public Coffee::Components::RestrictedSubsystem<system, system_manifest>,
    public gleam::api
{
    using reset_status = gleam::context::api::reset_status;

  public:
    using type = system;

    system() : context_checker{context_robustness()}
    {
        priority = 10;
    }

    void load_context_checker()
    {
        context_notifies = context_checker.reset_strategy()
                           == gleam::context::api::strategy::lose_on_reset;
    }

    void start_restricted(Proxy& proxy, time_point const& timepoint)
    {
        if(!context_checker.reset_state_supported())
            return;
        if(!context_notifies.has_value() || *context_notifies)
            return;
        auto lost_state = context_checker.context_state();
        if(lost_state == reset_status::no_error)
            return;

        auto state_string = magic_enum::enum_name(lost_state);
        // TODO: Trigger a reload
        Throw(undefined_behavior(
            std::string(state_string.begin(), state_string.end())));
    }
    void end_restricted(Proxy& proxy, time_point const& timepoint)
    {
    }

    gleam::context::api& context_checker;
    std::optional<bool>  context_notifies;
};

} // namespace gleam
