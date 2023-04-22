#pragma once

#include "entity_container.h"

#include <peripherals/stl/types.h>

namespace compo {

template<typename T, bool Reversed>
struct service_query
{
    using subsystem_iterator = typename std::conditional<
        Reversed,
        typename decltype(EntityContainer::subsystems)::const_reverse_iterator,
        typename decltype(EntityContainer::subsystems)::const_iterator>::type;
    using subsystem_ref =
        typename decltype(EntityContainer::subsystems)::const_reference;
    using service_predicate = std::function<bool(subsystem_ref)>;

    struct begin_iterator_t
    {
    };
    struct end_iterator_t
    {
    };

    service_query(EntityContainer& c, begin_iterator_t) :
        pred([](subsystem_ref e) { return C_DCAST<T>(e.second.get()); }),
        m_container(&c)
    {
        initialize_iterator();
    }

    service_query(EntityContainer& c, end_iterator_t) :
        pred([](subsystem_ref e) { return C_DCAST<T>(e.second.get()); }),
        m_container(&c)
    {
        it = end_iterator();
    }

    service_query& operator++()
    {
        it = std::find_if(++it, end_iterator(), pred);
        return *this;
    }

    bool operator==(service_query const& other) const
    {
        return other.it == it;
    }

    T& operator*() const
    {
        if(it == end_iterator())
            Throw(std::out_of_range("bad iterator"));

        return *C_DCAST<T>(it->second.get());
    }

  private:
    auto begin_iterator() const
    {
        if constexpr(Reversed)
            return m_container->subsystems.rbegin();
        else
            return m_container->subsystems.begin();
    }
    auto end_iterator() const
    {
        if constexpr(Reversed)
            return m_container->subsystems.rend();
        else
            return m_container->subsystems.end();
    }

    void initialize_iterator()
    {
        it = std::find_if(begin_iterator(), end_iterator(), pred);
    }

    service_predicate const pred;
    EntityContainer const*  m_container;
    subsystem_iterator      it;
};

} // namespace compo
