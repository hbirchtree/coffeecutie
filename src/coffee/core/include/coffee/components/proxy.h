#pragma once

#include <peripherals/stl/type_list.h>

#include "entity_container.h"

namespace compo {

using type_safety::empty_list_t;
using type_safety::type_list_t;
using type_safety::type_list::type_in_list_v;

struct ContainerProxy : stl_types::non_copy
{
    friend struct EntityContainer;
    template<typename T1, typename T2>
    friend struct EntityVisitor;
    friend struct SubsystemBase;

    FORCEDINLINE
    quick_container<EntityContainer::entity_query> select(u64 tags)
    {
        return m_container.select(tags);
    }

    template<is_component_tag ComponentType>
    FORCEDINLINE quick_container<EntityContainer::entity_query> select()
    {
        return m_container.select<ComponentType>();
    }

    template<typename Matcher>
    FORCEDINLINE quick_container<EntityContainer::entity_query> match()
    {
        return m_container.match<Matcher>();
    }

    template<typename ContainerType>
    FORCEDINLINE EntityRef<ContainerType> ref(Entity const& e)
    {
        return EntityRef<ContainerType>(e.id, C_CAST<ContainerType*>(this));
    }

    template<typename ContainerType>
    FORCEDINLINE EntityRef<ContainerType> ref(u64 e)
    {
        return EntityRef<ContainerType>(e, C_CAST<ContainerType*>(this));
    }

    auto& underlying()
    {
        return m_container;
    }

  protected:
    u64              current_entity;
    EntityContainer& m_container;

    ContainerProxy(EntityContainer& container) :
        current_entity(0), m_container(container)
    {
    }
};

template<
    typename ComponentList,
    typename SubsystemList,
    typename ServiceList = type_safety::empty_list_t>
struct ConstrainedProxy : ContainerProxy
{
    using component_list = ComponentList;
    using subsystem_list = SubsystemList;
    using service_list   = ServiceList;

    ConstrainedProxy(EntityContainer& container) : ContainerProxy(container)
    {
    }

    FORCEDINLINE quick_container<EntityContainer::entity_query> select(u64 tags)
    {
        return m_container.select(tags);
    }

    template<is_component_tag ComponentType>
    requires type_in_list_v<ComponentType, ComponentList>
        FORCEDINLINE quick_container<EntityContainer::entity_query> select()
    {
        return ContainerProxy::select<ComponentType>();
    }

    template<is_component_tag ComponentType>
    requires type_in_list_v<ComponentType, ComponentList>
        FORCEDINLINE typename ComponentType::value_type* get(u64 id)
    {
        return m_container.get<ComponentType>(id);
    }

    template<is_component_tag ComponentType>
    requires type_in_list_v<ComponentType, ComponentList>
        FORCEDINLINE typename ComponentType::value_type const* get(u64 id) const
    {
        return m_container.get<ComponentType>(id);
    }

    template<is_component_tag ComponentType>
    requires type_in_list_v<ComponentType, ComponentList>
        FORCEDINLINE typename ComponentType::value_type& get()
    {
        auto v = get<ComponentType>(current_entity);

        if constexpr(compile_info::debug_mode)
            if(!v)
                Throw(undefined_behavior("component not found"));

        return *v;
    }

    template<is_component_tag ComponentType>
    requires type_in_list_v<ComponentType, ComponentList>
        FORCEDINLINE typename ComponentType::value_type const& get() const
    {
        auto v = get<ComponentType>(current_entity);

        if constexpr(compile_info::debug_mode)
            if(!v)
                Throw(undefined_behavior("component not found"));

        return *v;
    }

    template<is_tag_type SubsystemType>
    requires type_in_list_v<SubsystemType, SubsystemList>
        FORCEDINLINE typename SubsystemType::type& subsystem()
    {
        return m_container.subsystem_cast<SubsystemType>();
    }

    template<is_tag_type SubsystemType>
    requires type_in_list_v<SubsystemType, SubsystemList>
        //
        FORCEDINLINE void subsystem(SubsystemType*& target)
    {
        target = &m_container.subsystem_cast<SubsystemType>();
    }

    template<is_tag_type SubsystemType>
    requires type_in_list_v<SubsystemType, SubsystemList>
        //
        FORCEDINLINE void subsystem(SubsystemType const*& target)
    {
        target = &m_container.subsystem_cast<SubsystemType>();
    }

    template<is_tag_type Service>
    requires type_in_list_v<Service, ServiceList>
        FORCEDINLINE typename Service::type* service()
    {
        return m_container.service<Service>();
    }

    template<class BaseType>
    FORCEDINLINE int services_with()
    {
        return m_container.services_with<BaseType>();
    }
};

template<typename Manifest>
using proxy_of = ConstrainedProxy<
    typename Manifest::components,
    typename Manifest::subsystems,
    typename Manifest::services>;

} // namespace compo
