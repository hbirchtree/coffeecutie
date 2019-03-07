#pragma once

#include <coffee/components/entity_container.h>
#include <coffee/components/visitor.h>

namespace Coffee {
namespace Components {

namespace detail {

using node_id = size_t;

struct visitor_path
{
    static constexpr node_id sentinel = std::numeric_limits<node_id>::max();

    node_id   nodes[2];
    type_hash association;
};

FORCEDINLINE void explore_graph(
    Set<node_id>& out, Vector<visitor_path> const& graph, node_id current_node)
{
    for(auto const& node : graph)
        if(node.nodes[0] == current_node)
            if(node.nodes[1] != visitor_path::sentinel)
            {
                out.insert(node.nodes[1]);
                explore_graph(out, graph, node.nodes[1]);
            }
}

FORCEDINLINE Vector<visitor_path> create_visitor_graph(
    Vector<UqPtr<EntityVisitorBase>> const& visitors)
{
    Vector<visitor_path> graph;

    Map<type_hash, Vector<node_id>> buckets;

    /* Add visitors to buckets */
    node_id idx = 0;
    for(auto const& visitor : visitors)
    {
        for(auto const& component : visitor->components)
            buckets[component].push_back(idx);

        idx++;
    }

    auto neigh_size = visitors.size();
    Vector<bool> neighbor_matrix;
    neighbor_matrix.resize(visitors.size() * visitors.size());

    /* Create a minimal spanning tree between all visitors in bucket */
    for(auto const& bucket : buckets)
    {
        /* TODO: Handle visitors with exclusive access */
        if(bucket.second.size() < 2)
        {
            auto node_idx = bucket.second.at(0);
            neighbor_matrix.at(neigh_size * node_idx + node_idx) = true;
            continue;
        }

        auto start = bucket.second.begin();
        auto it1 = start;
        auto it2 = start + 1;

        for(; it2 != bucket.second.end(); it1++, it2++)
        {
            auto x = it1 - start;
            auto y = it2 - start;

            bool val = true;

            auto coord1 = neigh_size * y + x;
            auto coord2 = neigh_size * x + y;

            neighbor_matrix.at(coord1) = neighbor_matrix.at(coord1) | val;
            neighbor_matrix.at(coord2) = neighbor_matrix.at(coord2) | val;
        }
    }

    return graph;
}

} // namespace detail

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

Vector<detail::visitor_path> EntityContainer::create_task_graph()
{
    return detail::create_visitor_graph(visitors);
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
