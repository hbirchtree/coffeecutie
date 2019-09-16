#pragma once

#include "entity_container.h"

#include <coffee/core/stl_types.h>

namespace Coffee {
namespace Components {

template<typename T>
struct service_query : Iterator<ForwardIteratorTag, T>
{
    using subsystem_iterator =
        decltype(EntityContainer::subsystems)::const_iterator;
    using subsystem_ref = decltype(EntityContainer::subsystems)::const_reference;
    using service_predicate = Function<bool(subsystem_ref)>;

    struct begin_iterator
    {
    };
    struct end_iterator
    {
    };

    service_query(EntityContainer& c, begin_iterator) :
        pred([](subsystem_ref e) {
            return C_DCAST<T>(e.second.get());
        }),
        m_container(&c)
    {
        initialize_iterator();
    }

    service_query(EntityContainer& c, end_iterator) : m_container(&c)
    {
    }

    service_query& operator++()
    {
        it = std::find_if(++it, m_container->subsystems.end(), pred);
        return *this;
    }

    bool operator==(service_query const& other)
    {
        return other.it == it;
    }

    bool operator!=(service_query const& other)
    {
        return other.it != it;
    }

    T& operator*() const
    {
        if(it == m_container->subsystems.end())
            Throw(std::out_of_range("bad iterator"));

        return *C_DCAST<T>(it->second.get());
    }

  private:
    void initialize_iterator()
    {
        it = std::find_if(
            m_container->subsystems.begin(),
            m_container->subsystems.end(),
            pred);
    }

    service_predicate const pred;
    EntityContainer const*  m_container;
    subsystem_iterator      it;
};

} // namespace Components
} // namespace Coffee
