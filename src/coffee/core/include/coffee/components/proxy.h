#pragma once

#include <peripherals/stl/type_list.h>

#include "entity_container.h"

namespace Coffee::Components {

struct ContainerProxy : non_copy
{
    friend struct EntityContainer;
    template<typename T1, typename T2>
    friend struct EntityVisitor;
    friend struct SubsystemBase;

    FORCEDINLINE
    quick_container<EntityContainer::entity_query> select(u32 tags)
    {
        return m_container.select(tags);
    }

    template<is_component ComponentType>
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
    FORCEDINLINE EntityRef<ContainerType> ref(Entity& e)
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
    typename ServiceList = TypeList<void>>
struct ConstrainedProxy : ContainerProxy
{
    using component_list = ComponentList;
    using subsystem_list = SubsystemList;
    using service_list   = ServiceList;

    ConstrainedProxy(EntityContainer& container) : ContainerProxy(container)
    {
    }

    FORCEDINLINE quick_container<EntityContainer::entity_query> select(u32 tags)
    {
        return m_container.select(tags);
    }

    template<typename ComponentType>
    requires type_list::type_in_list_v<ComponentType, ComponentList>
        FORCEDINLINE quick_container<EntityContainer::entity_query> select()
    {
        return ContainerProxy::select<ComponentType>();
    }

    template<typename ComponentType>
    requires type_list::type_in_list_v<ComponentType, ComponentList>
        FORCEDINLINE typename ComponentType::type* get(u64 id)
    {
        return m_container.get<ComponentType>(id);
    }

    template<typename ComponentType>
    requires type_list::type_in_list_v<ComponentType, ComponentList>
        FORCEDINLINE typename ComponentType::type const* get(u64 id) const
    {
        return m_container.get<ComponentType>(id);
    }

    template<typename ComponentType>
    requires type_list::type_in_list_v<ComponentType, ComponentList>
        FORCEDINLINE typename ComponentType::type& get()
    {
        auto v = get<ComponentType>(current_entity);

        if constexpr(compile_info::debug_mode)
            if(!v)
                Throw(undefined_behavior("component not found"));

        return *v;
    }

    template<typename ComponentType>
    requires type_list::type_in_list_v<ComponentType, ComponentList>
        FORCEDINLINE typename ComponentType::type const& get() const
    {
        auto v = get<ComponentType>(current_entity);

        if constexpr(compile_info::debug_mode)
            if(!v)
                Throw(undefined_behavior("component not found"));

        return *v;
    }

    template<typename OutputType>
    requires type_list::type_in_list_v<OutputType, SubsystemList> FORCEDINLINE
        Subsystem<OutputType>
    &subsystem()
    {
        return m_container.subsystem<OutputType>();
    }

    template<typename SubsystemType>
    requires type_list::
        type_in_list_v<typename SubsystemType::tag_type, SubsystemList>
            FORCEDINLINE SubsystemType& subsystem_cast()
    {
        return m_container.subsystem_cast<SubsystemType>();
    }

    template<typename Service>
    requires type_list::type_in_list_v<Service, ServiceList>
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

} // namespace Coffee::Components
