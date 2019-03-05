#pragma once

#include "components.h"

namespace Coffee {
namespace Components {

struct ContainerProxy : non_copy
{
    friend struct EntityContainer;
    template<typename T1, typename T2>
    friend struct EntityVisitor;

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

    template<typename OutputType>
    FORCEDINLINE Subsystem<OutputType>& subsystem()
    {
        return m_container.subsystem<OutputType>();
    }

    template<typename ComponentType>
    typename ComponentType::type* get(u64 id)
    {
        return m_container.get<ComponentType>(id);
    }

    template<typename ComponentType>
    FORCEDINLINE typename ComponentType::type& get()
    {
        typename ComponentType::type* v = get<ComponentType>(current_entity);

#if MODE_DEBUG
        if(!v)
            Throw(undefined_behavior("component not found"));
#endif

        return *v;
    }

  protected:
    u64              current_entity;
    EntityContainer& m_container;

    ContainerProxy(EntityContainer& container) :
        current_entity(0), m_container(container)
    {
    }
};

template<typename ComponentList, typename SubsystemList>
struct ConstrainedProxy : ContainerProxy
{
    ConstrainedProxy(EntityContainer& container) : ContainerProxy(container)
    {
    }

    template<typename ComponentType>
    FORCEDINLINE typename ComponentType::type* get(u64 id)
    {
        type_list::type_in_list<ComponentType, ComponentList>();
        return ContainerProxy::get<ComponentType>(id);
    }

    template<typename ComponentType>
    FORCEDINLINE typename ComponentType::type& get()
    {
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
        return ContainerProxy::subsystem<OutputType>();
    }
};

}
}
