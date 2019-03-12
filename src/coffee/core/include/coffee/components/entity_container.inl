#pragma once

#include <coffee/components/entity_container.h>
#include <coffee/components/visitor.h>

namespace Coffee {
namespace Components {

namespace detail {

using node_id = size_t;

void assign_reachables(
    Vector<bool>& matrix,
    size_t        size,
    node_id       source_node,
    node_id       current_node)
{
    for(auto i : Range<>(size))
        if(matrix[size * current_node + i] && !matrix[size * source_node + i])
        {
            matrix[size * source_node + i] = true;
            assign_reachables(matrix, size, source_node, i);
        }
}

FORCEDINLINE void expand_neighbor_matrix(
    Vector<bool>& matrix, size_t size, node_id current_node)
{
    for(auto i : Range<>(size))
        if(i != current_node && matrix[size * current_node + i])
            assign_reachables(matrix, size, current_node, i);
}

FORCEDINLINE EntityContainer::visitor_graph create_visitor_graph(
    Vector<UqPtr<EntityVisitorBase>> const& visitors)
{
    Map<type_hash, Vector<node_id>> buckets;

    /* Add visitors to buckets */
    node_id idx = 0;
    for(auto const& visitor : visitors)
    {
        for(auto const& component : visitor->components)
            buckets[component].push_back(idx);

        idx++;
    }

    auto         neigh_size = visitors.size();
    Vector<bool> neighbor_matrix;
    neighbor_matrix.resize(visitors.size() * visitors.size());

    /* Create a neighbor matrix for the nodes */
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
        auto it1   = start;
        auto it2   = start + 1;

        for(; it2 != bucket.second.end(); it1++, it2++)
        {
            auto x = C_FCAST<size_t>(it1 - start);
            auto y = C_FCAST<size_t>(it2 - start);

            bool val = true;

            auto coord1 = neigh_size * y + x;
            auto coord2 = neigh_size * x + y;

            neighbor_matrix.at(coord1) = neighbor_matrix.at(coord1) | val;
            neighbor_matrix.at(coord2) = neighbor_matrix.at(coord2) | val;
        }
    }

    /* Connect all nodes that run on main thread, grouping them together */
    {
        auto const is_mainthread = [](UqPtr<EntityVisitorBase> const& p) {
            return (p->flags & VisitorFlags::MainThread) ==
                   VisitorFlags::MainThread;
        };

        auto start = visitors.begin();

        auto it1 = std::find_if(start, visitors.end(), is_mainthread);

        while(it1 != visitors.end())
        {
            auto new_start = it1 + 1;
            auto it2 = std::find_if(new_start, visitors.end(), is_mainthread);

#if MODE_DEBUG
            if(it1 == it2)
                Throw(undefined_behavior("incorrect grouping"));
#endif
            if(it2 == visitors.end())
                break;

            auto x = C_FCAST<size_t>(it1 - start);
            auto y = C_FCAST<size_t>(it2 - start);

            neighbor_matrix.at(neigh_size * y + x) = true;
            neighbor_matrix.at(neigh_size * x + y) = true;

            it1 = it2;
        }
    }

    /* Expand neighbor matrix completely */
    for(auto i : Range<>(neigh_size))
        expand_neighbor_matrix(neighbor_matrix, neigh_size, i);

    /* De-duplicate neighbor matrix rows to create task queues */
    Set<Vector<bool>> unique_rows;
    for(auto i : Range<>(neigh_size))
    {
        unique_rows.insert(Vector<bool>(
            neighbor_matrix.begin() + C_FCAST<ssize_t>(neigh_size * i),
            neighbor_matrix.begin() + C_FCAST<ssize_t>(neigh_size * (i + 1))));
    }

    return unique_rows;
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

EntityContainer::visitor_graph EntityContainer::create_task_graph()
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
