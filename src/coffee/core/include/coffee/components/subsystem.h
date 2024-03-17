#pragma once

#include "entity_container.inl"
#include "proxy.h"
#include "types.h"

namespace compo {

template<typename T>
concept is_subsystem_manifest =
    type_safety::is_type_list<typename T::components> &&
    type_safety::is_type_list<typename T::subsystems> &&
    type_safety::is_type_list<typename T::services>;

template<typename T, typename Manifest>
concept is_restricted_subsystem =
    is_subsystem_manifest<Manifest> &&
    requires(T& v, typename compo::proxy_of<Manifest>& p) {
        {
            v.start_restricted(p, SubsystemBase::time_point())
        };
        {
            v.end_restricted(p, SubsystemBase::time_point())
        };
    };

template<typename ComponentList, typename SubsystemList, typename ServiceList>
struct SubsystemManifest
{
    using components = ComponentList;
    using subsystems = SubsystemList;
    using services   = ServiceList;
};

template<typename OuterType, is_subsystem_manifest Manifest>
// requires is_restricted_subsystem<OuterType, Manifest>
struct RestrictedSubsystem : SubsystemBase
{
    using Proxy = ConstrainedProxy<
        typename Manifest::components,
        typename Manifest::subsystems,
        typename Manifest::services>;

    virtual void start_frame(ContainerProxy& proxy, time_point const& t) final
    {
        Proxy p(this->get_container(proxy));
        auto* this_specialization = C_CAST<OuterType*>(this);
        this_specialization->start_restricted(p, t);
    }

    virtual void end_frame(ContainerProxy& proxy, time_point const& t) final
    {
        Proxy p(this->get_container(proxy));
        auto* this_specialization = C_CAST<OuterType*>(this);
        this_specialization->end_restricted(p, t);
    }
};

} // namespace compo
