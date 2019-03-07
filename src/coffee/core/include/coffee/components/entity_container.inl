#pragma once

#include "components.h"

namespace Coffee {
namespace Components {

FORCEDINLINE
void EntityContainer::exec()
{
    time_point     time_now = clock::now();
    ContainerProxy proxy(*this);

    for(auto& subsys : subsystems)
        subsys.second->start_frame(proxy, time_now);

    /* TODO: Put visitors in buckets according to what data they access */
    for(auto const& visitor : visitors)
    {
        visitor->dispatch(*this, time_now);
    }

    for(auto& subsys : subsystems)
        subsys.second->end_frame(proxy, time_now);
}

template<typename ComponentType>
FORCEDINLINE void EntityContainer::register_component(
    UqPtr<ComponentContainer<ComponentType>>&& c)
{
    static const type_hash type_id = typeid(ComponentType).hash_code();

    if(components.find(type_id) != components.end())
        Throw(implementation_error("cannot register type twice"));

    auto adapted = C_DCAST<ComponentContainerBase>(c.get());

    if(C_RCAST<void*>(adapted) != C_RCAST<void*>(c.get()))
        Throw(implementation_error("pointer casts will fail"));

    components.emplace(type_id, std::move(c));
}

} // namespace Components
} // namespace Coffee
