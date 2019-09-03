#pragma once

#include <peripherals/stl/type_list.h>

#include "entity_container.h"

namespace Coffee {
namespace Components {

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

    template<typename ComponentType>
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
    FORCEDINLINE quick_container<EntityContainer::entity_query> select()
    {
        type_list::type_in_list<ComponentType, ComponentList>();

        return ContainerProxy::select<ComponentType>();
    }

    template<typename ComponentType>
    FORCEDINLINE typename ComponentType::type* get(u64 id)
    {
        type_list::type_in_list<ComponentType, ComponentList>();

        return m_container.get<ComponentType>(id);
    }

    template<typename ComponentType>
    FORCEDINLINE typename ComponentType::type const* get(u64 id) const
    {
        type_list::type_in_list<ComponentType, ComponentList>();

        return m_container.get<ComponentType>(id);
    }

    template<typename ComponentType>
    FORCEDINLINE typename ComponentType::type& get()
    {
        type_list::type_in_list<ComponentType, ComponentList>();
        auto v = get<ComponentType>(current_entity);

#if MODE_DEBUG
        if(!v)
            Throw(undefined_behavior("component not found"));
#endif

        return *v;
    }

    template<typename ComponentType>
    FORCEDINLINE typename ComponentType::type const& get() const
    {
        type_list::type_in_list<ComponentType, ComponentList>();
        auto v = get<ComponentType>(current_entity);

#if MODE_DEBUG
        if(!v)
            Throw(undefined_behavior("component not found"));
#endif

        return *v;
    }

    template<typename OutputType>
    FORCEDINLINE Subsystem<OutputType>& subsystem()
    {
        type_list::type_in_list<OutputType, SubsystemList>();
        return m_container.subsystem<OutputType>();
    }

    template<typename SubsystemType>
    FORCEDINLINE SubsystemType& subsystem_cast()
    {
        type_list::
            type_in_list<typename SubsystemType::tag_type, SubsystemList>();
        return m_container.subsystem_cast<SubsystemType>();
    }

    template<typename Service>
    FORCEDINLINE Service* service()
    {
        type_list::type_in_list<Service, ServiceList>();
        return m_container.service<Service>();
    }

    template<class BaseType>
    FORCEDINLINE int services_with()
    {
        return m_container.services_with<BaseType>();
    }
};

} // namespace Components
} // namespace Coffee
