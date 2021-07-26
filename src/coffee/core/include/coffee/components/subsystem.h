#pragma once

#include "entity_container.inl"
#include "proxy.h"
#include "types.h"

namespace Coffee::Components {

template<typename T>
struct RestrictedSubsystem : SubsystemBase
{
    using OuterType = typename T::type;
    using Proxy     = ConstrainedProxy<
        typename OuterType::components,
        typename OuterType::subsystems,
        typename OuterType::services>;

    virtual void start_frame(ContainerProxy& proxy, time_point const& t) final
    {
        Proxy p(this->get_container(proxy));
        auto* this_specialization = C_CAST<OuterType>(this);
        this_specialization->start_restricted(p, t);
    }

    virtual void end_frame(ContainerProxy& proxy, time_point const& t) final
    {
        Proxy p(this->get_container(proxy));
        auto* this_specialization = C_CAST<OuterType>(this);
        this_specialization->end_restricted(p, t);
    }
};

} // namespace Coffee::Components
