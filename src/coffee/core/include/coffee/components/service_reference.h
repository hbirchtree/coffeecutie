#pragma once

#include "entity_container.h"

namespace Coffee {
namespace Components {

template<typename Service>
struct ServiceRef
{
    ServiceRef(EntityContainer* container) : m_container(container)
    {
    }

    Service* lock()
    {
        return m_container->service<Service>();
    }

    Service& operator*()
    {
        auto ptr = m_container->service<Service>();
        C_PTR_CHECK(ptr);
        return *ptr;
    }

    operator Service*()
    {
        return lock();
    }

  private:
    EntityContainer* m_container;
};

} // namespace Components
} // namespace Coffee
