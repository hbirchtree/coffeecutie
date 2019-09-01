#pragma once

#include "proxy.h"
#include "types.h"

namespace Coffee {
namespace Components {

template<typename T, typename CompList, typename SubsysList>
struct RestrictedSubsystem : Subsystem<T>
{
    using Proxy = ConstrainedProxy<CompList, SubsysList>;

    virtual void start_frame(
        ContainerProxy& proxy, time_point const& t) override final
    {
        Proxy p(this->get_container(proxy));
        start_restricted(p, t);
    }

    virtual void end_frame(
        ContainerProxy& proxy, time_point const& t) override final
    {
        Proxy p(this->get_container(proxy));
        end_restricted(p, t);
    }

    virtual void start_restricted(Proxy&, time_point const&)
    {
    }

    virtual void end_restricted(Proxy&, time_point const&)
    {
    }
};

} // namespace Components
} // namespace Coffee
