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

    auto* lock()
    {
        return m_container->service<Service>();
    }

    auto& operator*()
    {
        auto ptr = m_container->service<Service>();
        C_PTR_CHECK(ptr);
        return *ptr;
    }

    operator typename Service::type*()
    {
        return lock();
    }

  private:
    EntityContainer* m_container;
};

} // namespace Components
} // namespace Coffee
