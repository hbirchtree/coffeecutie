#pragma once

#include <peripherals/constants.h>
#include <peripherals/stl/source_location.h>
#include <peripherals/stl/standard_exceptions.h>
#include <platforms/pimpl_state.h>
#include <thread>

#ifndef COFFEE_COMPONENT_NAME
#define COFFEE_COMPONENT_NAME "(unknown)"
#endif

namespace platform {
namespace profiling {

using namespace ::libc_types;
using namespace ::stl_types;

using PClock     = std::chrono::high_resolution_clock;
using PExtraData = std::map<std::string, std::string>;

struct datapoint_t
{
    using duration_t = PClock::duration;

    enum type_t
    {
        profile,
        push,
        pop,
        complete,
    };

    enum attr_t
    {
        none_attr       = 0x0,
        explicit_thread = 0x1,
        async           = 0x2,
    };

    datapoint_t(
        type_t type, std::string_view name, PClock::duration duration = {})
        : tid(std::hash<std::thread::id>()(std::this_thread::get_id()))
        , name(name.begin(), name.end())
        , component(COFFEE_COMPONENT_NAME)
        , ts(PClock::now().time_since_epoch())
        , flags{
              .type  = type,
              .attrs = none_attr,
          }
    {
    }

    u64              tid;
    std::string      name;
    std::string_view component;
    std::string      thread_name;
    duration_t       ts;
    duration_t       dur;

    struct
    {
        type_t type : 3;
        attr_t attrs : 2;
        u16    _extra : 11;
        u16    _extra2;
        u32    _extra3;
    } flags;
};

struct ThreadInternalState
{
    virtual ~ThreadInternalState();
};

struct ThreadState
{
    GlobalState*                         writer;
    std::vector<std::string_view>        context_stack;
    u64                                  thread_id;
    std::unique_ptr<ThreadInternalState> internal_state;
};

struct PContext
{
    PContext()
        : start_time(PClock::now())
    {
        flags.enabled      = false;
        flags.deep_enabled = false;
    }

    PClock::time_point                          start_time;
    std::mutex                                  access;
    std::map<u64, std::shared_ptr<ThreadState>> thread_states;
    std::map<std::string, std::string>          extra_data;

    struct
    {
        std::atomic_bool enabled;
        std::atomic_bool deep_enabled;
    } flags;

    void enable()
    {
        Lock _(access);

        flags.enabled      = true;
        flags.deep_enabled = true;
    }

    void disable()
    {
        Lock _(access);

        flags.enabled      = false;
        flags.deep_enabled = false;
    }

    static std::shared_ptr<PContext> ProfilerStore()
    {
        return state->GetProfilerStore();
    }

    static std::shared_ptr<ThreadState> ProfilerTStore()
    {
        return state->GetProfilerTStore();
    }
};

struct RuntimeProperties : ThreadInternalState
{
    using PFunc = void (*)(ThreadState& ctxt, datapoint_t const& data);

    static auto& get_properties()
    {
        return *C_DCAST<RuntimeProperties>(
            PContext::ProfilerTStore()->internal_state.get());
    }

    static bool enabled()
    {
        if(!state || !state->ProfilerEnabled())
            return false;

        auto context = PContext::ProfilerStore();

        if(!context)
            return false;

        return context->flags.enabled;
    }

    static bool deep_enabled()
    {
        if(!enabled())
            return false;

        auto context = PContext::ProfilerStore();

        if(!context)
            return false;

        Lock _(context->access);
        return context->flags.deep_enabled;
    }

    std::shared_ptr<ThreadState> context;

    void push_stack(std::string_view frame)
    {
        PContext::ProfilerTStore()->context_stack.push_back(frame);
    }

    std::string_view pop_stack()
    {
        auto& thread_store = *PContext::ProfilerTStore();

        if(!thread_store.context_stack.size())
            return {};

        std::string_view out = thread_store.context_stack.back();
        thread_store.context_stack.pop_back();
        return out;
    }

    PFunc push;
};

struct ExtraDataImpl
{
    STATICINLINE void Add(
        UNUSED_PARAM(std::string const&, k),
        UNUSED_PARAM(std::string_view const&, v))
    {
        if constexpr(!compile_info::profiler::enabled)
            return;

        auto context = PContext::ProfilerStore();

        C_PTR_CHECK(context)

        Lock _(context->access);

        context->extra_data[k] = std::string(v.begin(), v.end());
    }

    STATICINLINE PExtraData Get()
    {
        if constexpr(!compile_info::profiler::enabled)
            return {};

        auto context = PContext::ProfilerStore();

        if(!context)
            return {};

        return context->extra_data;
    }
};

#if PERIPHERAL_PROFILER_ENABLED
template<bool library>
struct profile_wrapper
{
    STATICINLINE bool enabled()
    {
        if(library && !compile_info::profiler::deep_enabled)
            return false;
        else if(!compile_info::profiler::enabled)
            return false;

        if(!state || !state->ProfilerEnabled())
            return false;
        auto context = PContext::ProfilerStore();
        auto _ = std::unique_lock(context->access);
        if(library)
            return context->flags.deep_enabled;
        else
            return context->flags.enabled;
    }

    STATICINLINE void push(std::string_view name)
    {
        if(!enabled())
            return;

        auto props   = RuntimeProperties::get_properties();
        if(!props.context)
            return;
        props.push_stack(name);
        props.push(*props.context, datapoint_t(datapoint_t::push, name));
    }

    STATICINLINE void pop()
    {
        if(!enabled())
            return;

        auto props = RuntimeProperties::get_properties();
        auto name = props.pop_stack();
        if(name.empty())
            return;
        props.push(*props.context, datapoint_t(datapoint_t::pop, name));
    }

    STATICINLINE void profile(std::string_view name)
    {
        if(!enabled())
            return;

        auto props = RuntimeProperties::get_properties();
        props.push(*props.context, datapoint_t(datapoint_t::profile, name));
    }
};

struct SimpleProfilerImpl
{
    STATICINLINE void PushContext(
        std::string_view name, datapoint_t::attr_t = datapoint_t::none_attr)
    {
        profile_wrapper<false>::push(name);
    }

    STATICINLINE void PopContext()
    {
        profile_wrapper<false>::pop();
    }

    STATICINLINE void Profile(
        std::string_view name, datapoint_t::attr_t = datapoint_t::none_attr)
    {
        profile_wrapper<false>::profile(name);
    }

    STATICINLINE void DeepPushContext(
        std::string_view name, datapoint_t::attr_t = datapoint_t::none_attr)
    {
        profile_wrapper<true>::push(name);
    }

    STATICINLINE void DeepPopContext()
    {
        profile_wrapper<true>::pop();
    }

    STATICINLINE void DeepProfile(
        std::string_view name, datapoint_t::attr_t = datapoint_t::none_attr)
    {
        profile_wrapper<true>::profile(name);
    }
};
#else
struct SimpleProfilerImpl
{
    STATICINLINE void PushContext(
        std::string_view, DataPoint::Attr = DataPoint::AttrNone)
    {
    }

    STATICINLINE void PopContext()
    {
    }

    STATICINLINE void Profile(
        std::string_view, DataPoint::Attr = DataPoint::AttrNone)
    {
    }

    STATICINLINE void DeepPushContext(
        std::string_view, DataPoint::Attr = DataPoint::AttrNone)
    {
    }

    STATICINLINE void DeepPopContext()
    {
    }

    STATICINLINE void DeepProfile(
        std::string_view, DataPoint::Attr = DataPoint::AttrNone)
    {
    }

    C_DEPRECATED
    STATICINLINE void SetDeepProfileMode(bool)
    {
    }

    C_DEPRECATED
    STATICINLINE void AddExtraData(std::string const&, std::string_view const&)
    {
    }

    STATICINLINE PExtraData* ExtraInfo()
    {
        return nullptr;
    }

    STATICINLINE i64 StartTime()
    {
        return 0;
    }
};
#endif

struct ProfilerContext
{
    FORCEDINLINE ProfilerContext(
        std::string_view name = stl_types::source_location().function_name())
    {
        SimpleProfilerImpl::PushContext(name);
    }

    FORCEDINLINE ~ProfilerContext()
    {
        SimpleProfilerImpl::PopContext();
    }
};

struct DeepProfilerContext
{
    FORCEDINLINE DeepProfilerContext(
        std::string_view name = stl_types::source_location().function_name())
    {
        SimpleProfilerImpl::DeepPushContext(name);
    }

    FORCEDINLINE ~DeepProfilerContext()
    {
        SimpleProfilerImpl::DeepPopContext();
    }
};

template<typename QueryType>
struct GpuProfilerContext
{
    FORCEDINLINE GpuProfilerContext(
        std::shared_ptr<QueryType> query,
        std::string_view name = stl_types::source_location().function_name(),
        u64              gpu_thread = 0x8085)
        : m_thread(gpu_thread)
        , m_name(name)
        , m_query(query)
    {
        if constexpr(!compile_info::profiler::gpu::enabled)
            return;

        m_start = PClock::now();
        m_query->begin();
        push_event(m_name);
    }

    FORCEDINLINE ~GpuProfilerContext()
    {
        if constexpr(!compile_info::profiler::gpu::enabled)
            return;

        DeepProfilerContext _("GpuProfilerContext::Query stall");

        m_query->end();
        push_event(m_name, m_query->result());
    }

    FORCEDINLINE void push_event(
        cstring name, PClock::time_point::rep offset = 0)
    {
        auto props = RuntimeProperties::get_properties();

        datapoint_t event(offset ? datapoint_t::pop : datapoint_t::push, name);
        event.ts =
            (m_start + std::chrono::nanoseconds(offset)).time_since_epoch();
        event.tid = m_thread;

        props.push(*props.context, event);
    }

    u64                        m_thread;
    std::string_view           m_name;
    PClock::time_point         m_start;
    std::shared_ptr<QueryType> m_query;
};

} // namespace profiling
} // namespace platform

#undef current_thread_id
#undef profiler_data_store
#undef profiler_tstore
#undef PFTRACE
