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
concept has_start_restricted =
    is_subsystem_manifest<Manifest> &&
    requires(T& v, typename compo::proxy_of<Manifest>& p) {
        { v.start_restricted(p, SubsystemBase::time_point()) };
    };

template<typename T, typename Manifest>
concept has_end_restricted =
    is_subsystem_manifest<Manifest> &&
    requires(T& v, typename compo::proxy_of<Manifest>& p) {
        { v.end_restricted(p, SubsystemBase::time_point()) };
    };

template<typename T, typename Manifest>
concept is_restricted_subsystem =
    has_start_restricted<T, Manifest> || has_end_restricted<T, Manifest>;

template<typename ComponentList, typename SubsystemList, typename ServiceList>
struct SubsystemManifest
{
    using components = ComponentList;
    using subsystems = SubsystemList;
    using services   = ServiceList;
};

template<typename OuterType, is_subsystem_manifest Manifest>
struct RestrictedSubsystem : SubsystemBase
{
    using Proxy = ConstrainedProxy<
        typename Manifest::components,
        typename Manifest::subsystems,
        typename Manifest::services>;

    virtual void start_frame(ContainerProxy& proxy, time_point const& t) final
    {
        if constexpr(has_start_restricted<OuterType, Manifest>)
        {
            access::scope_guard _(this->runtime_access);
            Proxy p(this->get_container(proxy));
            auto* this_specialization = C_CAST<OuterType*>(this);
            this_specialization->start_restricted(p, t);
        } else
            SubsystemBase::start_frame(proxy, t);
    }

    virtual void end_frame(ContainerProxy& proxy, time_point const& t) final
    {
        if constexpr(has_end_restricted<OuterType, Manifest>)
        {
            access::scope_guard _(this->runtime_access);
            Proxy p(this->get_container(proxy));
            auto* this_specialization = C_CAST<OuterType*>(this);
            this_specialization->end_restricted(p, t);
        } else
            SubsystemBase::end_frame(proxy, t);
    }

    virtual std::vector<access::entry> const& declared_components()
        const override
    {
        static const auto set =
            access::collect<typename Manifest::components>();
        return set;
    }

    virtual std::vector<access::entry> const& declared_subsystems()
        const override
    {
        static const auto set =
            access::collect<typename Manifest::subsystems>();
        return set;
    }

    virtual std::vector<access::entry> const& declared_services() const override
    {
        static const auto set = access::collect<typename Manifest::services>();
        return set;
    }

    virtual bool declares_access() const override
    {
        return true;
    }
};

} // namespace compo
