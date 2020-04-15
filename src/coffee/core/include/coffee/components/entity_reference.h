#pragma once

#include "entity_container.h"

namespace Coffee {
namespace Components {

template<typename ContainerType>
struct EntityRef
{
    EntityRef()
    {
    }
    EntityRef(u64 source, ContainerType* container) :
        m_id(source), container(container)
    {
    }

    template<typename T>
    typename T::type& get()
    {
        auto ptr = container->template get<T>(m_id);
        C_PTR_CHECK(ptr);
        return *ptr;
    }

    u64 id() const
    {
        return m_id;
    }

    template<typename T>
    typename T::type const& get() const
    {
        /* TODO: Add const get<T>() function to EntityContainer */
        auto container_mut = C_CCAST<ContainerType*>(container);

        auto ptr = container_mut->template get<T>(m_id);
        C_PTR_CHECK(ptr);
        return *ptr;
    }

  private:
    u64            m_id;
    ContainerType* container;
};

template<typename ContainerType, typename ComponentType>
struct ComponentRef
{
    ComponentRef() : m_id(0), m_ref(nullptr)
    {
    }
    ComponentRef(u64 id, ContainerType* container) : m_id(id), m_ref(container)
    {
    }

    typename ComponentType::type& operator*() const
    {
        return *m_ref->template get<ComponentType>(m_id);
    }

    u64            m_id;
    ContainerType* m_ref;
};

} // namespace Components
} // namespace Coffee
