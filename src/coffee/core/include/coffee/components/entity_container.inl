#pragma once

#include "types.h"
#include <coffee/components/entity_container.h>
#include <coffee/components/entity_reference.h>
#include <coffee/components/scheduling.h>
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

FORCEDINLINE std::vector<sched::node> EntityContainer::build_schedule_nodes(
    std::vector<SubsystemBase*> const& sorted)
{
    /* Batches index into this, so it must be complete before batching */
    std::vector<sched::node> nodes;
    nodes.reserve(sorted.size());

    for(auto* subsys : sorted)
    {
        type_hash self = 0;
        for(auto const& entry : subsystems)
            if(entry.second.get() == subsys)
            {
                self = entry.first;
                break;
            }

        nodes.push_back({
            subsys,
            sched::access_of(self, *subsys),
            ENT_TYPE_NAME(*subsys),
            subsys->frame_time_ns,
        });
    }

    sched::propagate_main_thread(nodes);

    return nodes;
}

FORCEDINLINE std::string EntityContainer::schedule_report()
{
    std::vector<SubsystemBase*> sorted;
    sorted.reserve(subsystems.size());
    std::transform(
        std::cbegin(subsystems),
        std::cend(subsystems),
        std::back_inserter(sorted),
        detail::pointer_extract);
    std::sort(sorted.begin(), sorted.end(), detail::subsystem_sort);

    auto nodes   = build_schedule_nodes(sorted);
    auto batches = sched::build_batches(nodes);

    return sched::format_batches(nodes, batches) +
           "  " + std::to_string(worker_count()) + " worker threads\n" +
           sched::format_windows(
               nodes, batches, sched::build_windows(nodes, batches));
}

FORCEDINLINE void EntityContainer::set_worker_count(size_t count)
{
    if(worker_count() == count)
        return;

    /* Destruction joins, so no job outlives the pool it was queued on */
    workers.reset();

    if(count)
        workers = std::make_unique<sched::worker_pool>(count);

    scheduled.clear();
}

FORCEDINLINE void EntityContainer::update_schedule(
    std::vector<SubsystemBase*> const& sorted)
{
    bool dirty = sorted != scheduled;

    if(!dirty)
        for(size_t index = 0; index < schedule_nodes.size(); index++)
            if(sched::access_changed(
                   schedule_nodes.at(index).access, *sorted.at(index)))
            {
                dirty = true;
                break;
            }

    if(!dirty)
        return;

    scheduled        = sorted;
    schedule_nodes   = build_schedule_nodes(sorted);
    schedule_batches = sched::build_batches(schedule_nodes);
    schedule_windows = sched::build_windows(schedule_nodes, schedule_batches);

    schedule_opens.assign(schedule_batches.size(), {});
    schedule_closes.assign(schedule_batches.size(), {});
    schedule_offloads = false;
    for(size_t index = 0; index < schedule_windows.size(); index++)
    {
        auto const& span = schedule_windows.at(index);
        if(!span.offloaded())
            continue;
        schedule_opens.at(span.first).push_back(index);
        schedule_closes.at(span.last).push_back(index);
        schedule_offloads = true;
    }

    if(debug_flags & Verbose_Schedule)
        log(libc::io::io_handles::err,
            "Coffee::Components",
            "schedule changed:\n" +
                sched::format_batches(schedule_nodes, schedule_batches) +
                sched::format_windows(
                    schedule_nodes, schedule_batches, schedule_windows),
            semantic::debug::Severity::Information);
}

FORCEDINLINE void EntityContainer::exec_batched(
    ContainerProxy&   proxy,
    time_point const& time_now,
    bool              measure,
    bool              reverse,
    frame_hook        hook,
    std::string_view  hook_name)
{
    if(schedule_offloads)
    {
        workers->reset();
        schedule_jobs.assign(schedule_nodes.size(), 0);
    }

    auto run = [&](size_t index) {
        auto& subsys = *schedule_nodes.at(index).subsystem;

        /* Nothing to call: skip before paying for the profiling context */
        if(reverse ? subsys.no_end_hook : subsys.no_start_hook)
            return;

        auto frame_name = ENT_TYPE_NAME(subsys) + std::string(hook_name);
        DProfContext _(frame_name);

        if(!measure)
        {
            (subsys.*hook)(proxy, time_now);
            return;
        }

        auto begin = clock::now();
        (subsys.*hook)(proxy, time_now);
        subsys.frame_time_ns += static_cast<u64>(
            std::chrono::duration_cast<std::chrono::nanoseconds>(
                clock::now() - begin)
                .count());
    };

    /* Opens at first, closes at last - mirrored when unwinding the frame */
    auto const& opening = reverse ? schedule_closes : schedule_opens;
    auto const& closing = reverse ? schedule_opens : schedule_closes;

    size_t const stages = schedule_batches.size();

    for(size_t step = 0; step < stages; step++)
    {
        size_t const current = reverse ? stages - 1u - step : step;

        if(schedule_offloads)
            for(auto index : opening.at(current))
                schedule_jobs.at(index) =
                    workers->submit([&run, index]() { run(index); });

        for(auto member : schedule_batches.at(current).members)
            if(!schedule_windows.at(member).offloaded())
                run(member);

        if(schedule_offloads)
            for(auto index : closing.at(current))
                workers->wait(schedule_jobs.at(index));
    }

    /* Nothing escapes the pass it was submitted in */
    if(schedule_offloads)
        workers->wait_all();
}

FORCEDINLINE
void EntityContainer::exec()
{
    constexpr auto wrap_exceptions = compile_info::debug_mode && false;

    auto const measure_frames =
        sched::report_frame() || (debug_flags & Verbose_Schedule);

    frames_elapsed++;

    if(!workers_configured)
    {
        workers_configured = true;
        set_worker_count(sched::configured_worker_count());
    }

    if(auto target = sched::report_frame();
       measure_frames && frames_elapsed == std::max<u64>(target, 1))
    {
        log(libc::io::io_handles::err,
            "Coffee::Components",
            "frame schedule:\n" + schedule_report(),
            semantic::debug::Severity::Information);
    }

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

    std::erase_if(
        subsystems_,
        [](SubsystemBase* s) {
            return s->no_start_hook && s->no_end_hook;
        });

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

    /* The first frames run serially: a manifest escape is only visible once
     * the hook that makes it has run at least once, and the load phase is
     * where most of them happen */
    bool const batched =
        worker_count() > 0 && frames_elapsed > sched::warmup_frames();

    if(batched)
        update_schedule(subsystems_);

    if(batched)
        exec_batched(
            proxy,
            time_now,
            measure_frames,
            false,
            &SubsystemBase::start_frame,
            "::start_frame");
    else
        for(auto& subsys_ptr : subsystems_)
        {
            auto& subsys = *subsys_ptr;
            if(subsys.no_start_hook)
                continue;

            if constexpr(wrap_exceptions)
                ENT_DBG_TYPE(Verbose_Subsystems, "subsystem:start:", subsys)
            auto frame_name =
                ENT_TYPE_NAME(subsys) + std::string("::start_frame");
            DProfContext _(frame_name);

            if constexpr(wrap_exceptions)
                wrap_exception<std::exception>(
                    ex_handler(subsys),
                    &SubsystemBase::start_frame,
                    &subsys,
                    std::ref(proxy),
                    time_now);
            else if(measure_frames)
            {
                auto begin = clock::now();
                subsys.start_frame(proxy, time_now);
                subsys.frame_time_ns += static_cast<u64>(
                    std::chrono::duration_cast<std::chrono::nanoseconds>(
                        clock::now() - begin)
                        .count());
            } else
                subsys.start_frame(proxy, time_now);
        }

    /* TODO: Put visitors in buckets according to what data they access */
    for(auto const& visitor_ptr : visitors)
    {
        auto& visitor = *visitor_ptr;

        if constexpr(wrap_exceptions)
            ENT_DBG_TYPE(Verbose_Visitors, "visitor:dispatch:", visitor)
        auto frame_name = ENT_TYPE_NAME(visitor) + std::string("::dispatch");
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

    if(batched)
        exec_batched(
            proxy,
            time_now,
            measure_frames,
            true,
            &SubsystemBase::end_frame,
            "::end_frame");
    else
        for(auto it = subsystems_.rbegin(); it != subsystems_.rend(); ++it)
        {
            auto& subsys = *(*it);
            if(subsys.no_end_hook)
                continue;

            if constexpr(wrap_exceptions)
                ENT_DBG_TYPE(Verbose_Subsystems, "subsystem:end:", subsys)
            auto frame_name = ENT_TYPE_NAME(subsys) + std::string("::end_frame");
            DProfContext _(frame_name);

            if constexpr(wrap_exceptions)
                wrap_exception<std::exception>(
                    ex_handler(subsys),
                    &SubsystemBase::end_frame,
                    std::ref(subsys),
                    std::ref(proxy),
                    time_now);
            else if(measure_frames)
            {
                auto begin = clock::now();
                subsys.end_frame(proxy, time_now);
                subsys.frame_time_ns += static_cast<u64>(
                    std::chrono::duration_cast<std::chrono::nanoseconds>(
                        clock::now() - begin)
                        .count());
            } else
                subsys.end_frame(proxy, time_now);
        }

    for(auto const& callback : frame_end_callbacks)
    {
        if(callback)
            callback();
    }
    advance_frame();
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
    std::vector<u64> removed_ids;
    for(auto const& entity : entities)
    {
        if(predicate(entity))
            removed_ids.push_back(entity.id);
    }
    auto remove_it = std::remove_if(entities.begin(), entities.end(), predicate);
    for(auto& component : components)
    {
        for(auto id : removed_ids)
            component.second->unregister_entity(id);
    }
    entities.erase(remove_it, entities.end());
}

inline bool EntityContainer::exists(u64 id) const
{
    return std::find_if(entities.begin(), entities.end(), [id](Entity const& entity) {
            return entity.id == id;
        }) != entities.end();
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
