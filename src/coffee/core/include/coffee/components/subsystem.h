#pragma once

#include "proxy.h"
#include "types.h"

namespace Coffee {
namespace Components {

template<
    typename T,
    typename CompList    = TypeList<void>,
    typename SubsysList  = TypeList<void>,
    typename ServiceList = TypeList<void>>
struct RestrictedSubsystem : Subsystem<T>
{
    using Proxy = ConstrainedProxy<CompList, SubsysList, ServiceList>;

    virtual void start_frame(ContainerProxy& proxy, time_point const& t) final
    {
        Proxy p(this->get_container(proxy));
        start_restricted(p, t);
    }

    virtual void end_frame(ContainerProxy& proxy, time_point const& t) final
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
