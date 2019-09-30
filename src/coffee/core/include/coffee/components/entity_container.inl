#pragma once

#include <coffee/components/entity_container.h>
#include <coffee/components/entity_reference.h>
#include <coffee/components/service_query.h>
#include <coffee/components/visitor.h>

namespace Coffee {
namespace Components {

namespace detail {

using node_id = size_t;

extern void expand_neighbor_matrix(
    Vector<bool>& matrix, size_t size, node_id current_node);

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

FORCEDINLINE SubsystemBase* pointer_extract(
    Pair<const type_hash, UqPtr<SubsystemBase>> const& sub)
{
    return sub.second.get();
}

FORCEDINLINE bool subsystem_sort(
    SubsystemBase const* s1, SubsystemBase const* s2)
{
    return s1->priority < s2->priority;
}

} // namespace detail

FORCEDINLINE
void EntityContainer::exec()
{
    time_point     time_now = clock::now();
    ContainerProxy proxy(*this);

    Vector<SubsystemBase*> subsystems_;
    subsystems_.reserve(subsystems.size());

    std::transform(
        std::cbegin(subsystems),
        std::cend(subsystems),
        std::back_inserter(subsystems_),
        detail::pointer_extract);

    std::sort(subsystems_.begin(), subsystems_.end(), detail::subsystem_sort);

    for(auto& subsys : subsystems_)
        subsys->start_frame(proxy, time_now);

    /* TODO: Put visitors in buckets according to what data they access */
    for(auto const& visitor : visitors)
    {
        visitor->dispatch(*this, time_now);
    }

    for(auto it = subsystems_.rbegin(); it != subsystems_.rend(); ++it)
        (*it)->end_frame(proxy, time_now);
}

FORCEDINLINE EntityContainer::visitor_graph EntityContainer::create_task_graph()
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

#if MODE_DEBUG
template<typename ServiceType, typename SubsystemType>
struct service_test
{
    template<typename T>
    struct dynamic_test
    {
        void operator()(SubsystemType* sub)
        {
            C_PTR_CHECK_MSG(
                C_DCAST<typename T::type>(sub), "service cast mismatch");
        }
    };

    void check_subsystems(SubsystemType* sub)
    {
        type_list::for_each<typename ServiceType::services, dynamic_test>(sub);
    }
};
#endif

template<typename ServiceType, typename SubsystemType>
void EntityContainer::register_subsystem_services(SubsystemType* subsystem)
{
    static_assert(std::is_same<type_hash, size_t>::value, "Mismatched types");

    auto types = type_list::collect_list<typename ServiceType::services>();

#if MODE_DEBUG
    service_test<ServiceType, SubsystemType>().check_subsystems(subsystem);
#endif

    for(type_hash v : types)
    {
        services.insert({v, subsystem});
    }
}

FORCEDINLINE EntityRef<EntityContainer> EntityContainer::ref(Entity& entity)
{
    return EntityRef<EntityContainer>(entity.id, this);
}

FORCEDINLINE EntityRef<EntityContainer> EntityContainer::ref(u64 entity)
{
    return EntityRef<EntityContainer>(entity, this);
}

template<typename ComponentTag>
ComponentRef<EntityContainer, ComponentTag> EntityContainer::ref_comp(
    u64 entity)
{
    return ComponentRef<EntityContainer, ComponentTag>(entity, this);
}

template<typename Service>
ServiceRef<Service> EntityContainer::service_ref()
{
    return ServiceRef<Service>(this);
}

template<class BaseType>
quick_container<service_query<BaseType>> EntityContainer::services_with()
{
    using query_type = service_query<BaseType>;

    return {[this]() {
                return query_type(
                    *this, typename query_type::begin_iterator_t());
            },
            [this]() {
                return query_type(*this, typename query_type::end_iterator_t());
            }};
}

FORCEDINLINE EntityContainer& SubsystemBase::get_container(
    SubsystemBase::ContainerProxy& proxy)
{
    return proxy.m_container;
}

} // namespace Components
} // namespace Coffee
