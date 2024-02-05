#pragma once

#include <coffee/components/entity_container.h>
#include <coffee/components/entity_reference.h>
#include <coffee/components/service_query.h>
#include <coffee/components/visitor.h>
#include <coffee/core/CProfiling>
#include <coffee/core/debug/logging.h>

#include <peripherals/stl/range.h>
#include <peripherals/stl/type_list.h>
#include <platforms/stacktrace.h>

#include <algorithm>

#define ENT_TYPE_NAME(var) \
    platform::stacktrace::demangle::name(typeid(var).name())

#define ENT_DBG_TYPE(flag, prefix, var)          \
    if(debug_flags & flag)                       \
    {                                            \
        std::string dbg;                         \
        (dbg += prefix) += ENT_TYPE_NAME(var);   \
        log(libc::io::io_handles::err,           \
            "Coffee::Components",                \
            dbg,                                 \
            semantic::debug::Severity::Verbose); \
    }

namespace compo {

using Coffee::DProfContext;
using Coffee::Logging::log;

using type_safety::type_list::collect_list;
using type_safety::type_list::for_each;

namespace detail {

using node_id = size_t;

extern void expand_neighbor_matrix(
    std::vector<bool>& matrix, size_t size, node_id current_node);

FORCEDINLINE EntityContainer::visitor_graph create_visitor_graph(
    std::vector<std::unique_ptr<EntityVisitorBase>> const& visitors)
{
    std::map<type_hash, std::vector<node_id>> buckets;

    /* Add visitors to buckets */
    node_id idx = 0;
    for(auto const& visitor : visitors)
    {
        for(auto const& component : visitor->components)
            buckets[component].push_back(idx);

        idx++;
    }

    auto              neigh_size = visitors.size();
    std::vector<bool> neighbor_matrix;
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
        auto const is_mainthread
            = [](std::unique_ptr<EntityVisitorBase> const& p) {
                  return (p->flags & VisitorFlags::MainThread)
                         == VisitorFlags::MainThread;
              };

        auto start = visitors.begin();

        auto it1 = std::find_if(start, visitors.end(), is_mainthread);

        while(it1 != visitors.end())
        {
            auto new_start = it1 + 1;
            auto it2 = std::find_if(new_start, visitors.end(), is_mainthread);

            if constexpr(compile_info::debug_mode)
                if(it1 == it2)
                    Throw(undefined_behavior("incorrect grouping"));

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
    for(auto i : stl_types::Range<>(neigh_size))
        expand_neighbor_matrix(neighbor_matrix, neigh_size, i);

    /* De-duplicate neighbor matrix rows to create task queues */
    std::set<std::vector<bool>> unique_rows;
    for(auto i : stl_types::Range<>(neigh_size))
    {
        unique_rows.insert(std::vector<bool>(
            neighbor_matrix.begin() + static_cast<ptroff>(neigh_size * i),
            neighbor_matrix.begin()
                + static_cast<ptroff>(neigh_size * (i + 1))));
    }

    return unique_rows;
}

FORCEDINLINE SubsystemBase* pointer_extract(
    std::pair<const type_hash, std::unique_ptr<SubsystemBase>> const& sub)
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
    constexpr auto wrap_exceptions = compile_info::debug_mode && false;

    /* This timestamp is relative to the starting time, so that it becomes
     * useful for different operations that only need to keep track of time in
     * some manner
     * It should be suitable for running timers, and scheduling work
     * X seconds in the future and similar
     */
    time_point     time_now = relative_timestamp();
    ContainerProxy proxy(*this);

    std::vector<SubsystemBase*> subsystems_;
    subsystems_.reserve(subsystems.size());

    std::transform(
        std::cbegin(subsystems),
        std::cend(subsystems),
        std::back_inserter(subsystems_),
        detail::pointer_extract);

    std::sort(subsystems_.begin(), subsystems_.end(), detail::subsystem_sort);

    auto ex_handler = [](auto const& source) {
        return [&source](auto const& e) {
            log(libc::io::io_handles::err,
                "Components",
                platform::stacktrace::demangle::type_name(source) + ": "
                    + platform::stacktrace::demangle::type_name(e) + ": "
                    + e.what());
        };
    };

    for(auto& subsys_ptr : subsystems_)
    {
        auto& subsys = *subsys_ptr;

        if constexpr(wrap_exceptions)
            ENT_DBG_TYPE(Verbose_Subsystems, "subsystem:start:", subsys)
        auto frame_name = typeid(subsys).name() + std::string("::start_frame");
        DProfContext _(frame_name);

        if constexpr(wrap_exceptions)
            wrap_exception<std::exception>(
                ex_handler(subsys),
                &SubsystemBase::start_frame,
                &subsys,
                std::ref(proxy),
                time_now);
        else
            subsys.start_frame(proxy, time_now);
    }

    /* TODO: Put visitors in buckets according to what data they access */
    for(auto const& visitor_ptr : visitors)
    {
        auto& visitor = *visitor_ptr;

        if constexpr(wrap_exceptions)
            ENT_DBG_TYPE(Verbose_Visitors, "visitor:dispatch:", visitor)
        auto frame_name = typeid(visitor).name() + std::string("::dispatch");
        DProfContext _(frame_name);

        if constexpr(wrap_exceptions)
            wrap_exception<std::exception>(
                ex_handler(visitor_ptr),
                &EntityVisitorBase::dispatch,
                std::ref(visitor),
                std::ref(*this),
                time_now);
        else
            visitor.dispatch(*this, time_now);
    }

    for(auto it = subsystems_.rbegin(); it != subsystems_.rend(); ++it)
    {
        auto& subsys = *(*it);

        if constexpr(wrap_exceptions)
            ENT_DBG_TYPE(Verbose_Subsystems, "subsystem:end:", subsys)
        auto frame_name = typeid(subsys).name() + std::string("::end_frame");
        DProfContext _(frame_name);

        if constexpr(wrap_exceptions)
            wrap_exception<std::exception>(
                ex_handler(subsys),
                &SubsystemBase::end_frame,
                std::ref(subsys),
                std::ref(proxy),
                time_now);
        else
            subsys.end_frame(proxy, time_now);
    }
}

FORCEDINLINE EntityContainer::visitor_graph EntityContainer::create_task_graph()
{
    return detail::create_visitor_graph(visitors);
}

template<typename ComponentType>
FORCEDINLINE void EntityContainer::register_component(
    std::unique_ptr<ComponentContainer<ComponentType>>&& c)
{
    static const type_hash type_id = typeid(ComponentType).hash_code();

    if(components.find(type_id) != components.end())
        Throw(implementation_error("cannot register type twice"));

    auto adapted = C_DCAST<ComponentContainerBase>(c.get());

    if(C_RCAST<void*>(adapted) != C_RCAST<void*>(c.get()))
        Throw(implementation_error("pointer casts will fail"));

    components.emplace(type_id, std::move(c));
}

template<typename ServiceType, typename SubsystemType>
struct service_test
{
    struct dynamic_test
    {
        dynamic_test(SubsystemType* sub) : sub(sub)
        {
        }
        template<typename T>
        void operator()()
        {
            static_assert(std::is_base_of_v<typename T::type, SubsystemType>);
            C_PTR_CHECK_MSG(
                C_DCAST<typename T::type>(sub), "service cast mismatch");
        }

        SubsystemType* sub;
    };

    void check_subsystems(SubsystemType* sub)
    {
        for_each<typename ServiceType::services>(dynamic_test(sub));
    }
};

template<typename ServiceType, typename SubsystemType>
void EntityContainer::register_subsystem_services(SubsystemType* subsystem)
{
    static_assert(std::is_same<type_hash, size_t>::value, "Mismatched types");

    auto types = collect_list<typename ServiceType::services>();

    if constexpr(compile_info::debug_mode)
        service_test<ServiceType, SubsystemType>().check_subsystems(subsystem);

    for(type_hash v : types)
    {
        services.insert({v, subsystem});
    }
}

FORCEDINLINE EntityRef<EntityContainer> EntityContainer::ref(
    Entity const& entity)
{
    return EntityRef<EntityContainer>(entity.id, this);
}

FORCEDINLINE EntityRef<EntityContainer> EntityContainer::ref(u64 entity)
{
    return EntityRef<EntityContainer>(entity, this);
}

FORCEDINLINE EntityRef<EntityContainer> EntityContainer::create_entity(
    EntityRecipe const& recipe)
{
    entity_counter++;

    entities.emplace_back();
    auto& entity = entities.back();

    entity.id   = entity_counter;
    entity.tags = recipe.tags;

    for(auto type_id : recipe.components)
        add_component(entity_counter, type_id);

    return ref(entity);
}

FORCEDINLINE void EntityContainer::remove_entity_if(
    std::function<bool(Entity const&)>&& predicate)
{
    auto remove_it
        = std::remove_if(entities.begin(), entities.end(), predicate);
    auto removed_entities
        = semantic::Span<Entity>(&(*remove_it), entities.end() - remove_it);
    for(auto& component : components)
    {
        for(auto const& entity : removed_entities)
            component.second->unregister_entity(entity.id);
    }
    entities.erase(remove_it, entities.end());
}

inline u64 EntityContainer::tags_of(u64 id) const
{
    auto it = std::find_if(
        entities.begin(), entities.end(), [id](Entity const& entity) {
            return entity.id == id;
        });
    if(it == entities.end())
        return 0;
    return it->tags;
}

template<is_tag_type ComponentTag>
ComponentRef<EntityContainer, ComponentTag> EntityContainer::ref_comp(
    u64 entity)
{
    return ComponentRef<EntityContainer, ComponentTag>(entity, this);
}

template<is_subsystem Service>
ServiceRef<Service> EntityContainer::service_ref()
{
    return ServiceRef<Service>(this);
}

template<class BaseType, bool Reversed>
service_query<BaseType, Reversed> EntityContainer::services_with()
{
    return service_query<BaseType, Reversed>(*this);
}

template<class BaseType>
auto EntityContainer::services_with(reverse_query_t)
{
    return services_with<BaseType, true>();
}

template<class BaseType>
auto EntityContainer::services_with(service_sort_predicate<BaseType> sorter)
{
    auto                   services = services_with<BaseType>();
    std::sort(services.begin(), services.end(), std::move(sorter));
    return services;
}

FORCEDINLINE EntityContainer& SubsystemBase::get_container(
    SubsystemBase::ContainerProxy& proxy)
{
    return proxy.m_container;
}

} // namespace compo

#undef ENT_DBG_TYPE
#undef ENT_TYPE_NAME
