#pragma once

#include "entity_container.h"

#include <peripherals/stl/types.h>

namespace compo {

template<typename T, bool Reversed>
struct service_query : std::vector<T*>
{
    using subsystem_iterator =
        typename decltype(EntityContainer::subsystems)::const_iterator;
    using subsystem_ref =
        typename decltype(EntityContainer::subsystems)::const_reference;
    using service_predicate = std::function<bool(subsystem_ref)>;

    service_query(EntityContainer& c)
        : pred([](subsystem_ref e) { return C_DCAST<T>(e.second.get()); })
        , m_container(&c)
    {
        std::for_each(
            c.subsystems.begin(), c.subsystems.end(), [this](auto& sub) {
                if(pred(sub))
                    this->push_back(C_DCAST<T>(sub.second.get()));
            });
        const auto comparator = [](const T* s1_, const T* s2_) {
            auto const& s1 = dynamic_cast<SubsystemBase const*>(s1_);
            auto const& s2 = dynamic_cast<SubsystemBase const*>(s2_);
            if(Reversed)
                return s1->priority > s2->priority;
            else
                return s1->priority < s2->priority;
        };
        std::sort(this->begin(), this->end(), comparator);
    }

  private:
    service_predicate const pred;
    EntityContainer const*  m_container;
};

} // namespace compo
