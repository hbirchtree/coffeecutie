#pragma once

#include <peripherals/constants.h>
#include <peripherals/profiler/profiler.h>
#include <peripherals/stl/source_location.h>
#include <platforms/pimpl_state.h>

#ifndef COFFEE_COMPONENT_NAME
#define COFFEE_COMPONENT_NAME "(unknown)"
#endif

namespace platform {
namespace profiling {

using namespace ::libc_types;
using namespace ::stl_types;

#if PERIPHERAL_PROFILER_ENABLED
using profiler_compile_opts = ::profiler::options::compile<true, true>;
#else
using profiler_compile_opts = ::profiler::options::compile_default;
#endif

using PClock     = std::chrono::high_resolution_clock;
using PExtraData = std::map<std::string, std::string>;

struct ThreadInternalState
{
    virtual ~ThreadInternalState();
};

struct ThreadState
{
    GlobalState*                         writer;
    std::vector<std::string_view>        context_stack;
    ThreadId::Hash                       thread_id;
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

    PClock::time_point                                     start_time;
    std::mutex                                             access;
    std::map<ThreadId::Hash, std::shared_ptr<ThreadState>> thread_states;
    std::map<std::string, std::string>                     extra_data;

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

template<typename Context, typename Clock, typename Types>
struct RuntimeProperties : ThreadInternalState
{
    using ThisType = RuntimeProperties<Context, Clock, Types>;
    using PFunc =
        void (*)(Context& ctxt, typename Types::datapoint const& data);

    static ThisType& get_properties()
    {
        return *C_DCAST<ThisType>(
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

    FORCEDINLINE cstring component()
    {
        return COFFEE_COMPONENT_NAME;
    }

    PFunc push;
};

using Profiler = ::profiler::
    prof<ThreadState, PClock, RuntimeProperties, profiler_compile_opts>;

using DataPoint = Profiler::datapoint;

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
struct SimpleProfilerImpl
{
    STATICINLINE void PushContext(
        std::string_view name, DataPoint::Attr = DataPoint::AttrNone)
    {
        Profiler::app::push(name);
    }

    STATICINLINE void PopContext()
    {
        Profiler::app::pop();
    }

    STATICINLINE void Profile(
        std::string_view name, DataPoint::Attr = DataPoint::AttrNone)
    {
        Profiler::app::profile(name);
    }

    STATICINLINE void DeepPushContext(
        std::string_view name, DataPoint::Attr = DataPoint::AttrNone)
    {
        Profiler::lib::push(name);
    }

    STATICINLINE void DeepPopContext()
    {
        Profiler::lib::pop();
    }

    STATICINLINE void DeepProfile(
        std::string_view name, DataPoint::Attr = DataPoint::AttrNone)
    {
        Profiler::lib::profile(name);
    }

    C_DEPRECATED_S("Doesn't do anything")

    STATICINLINE void InitProfiler()
    {
    }

    C_DEPRECATED_S("use State::GetProfilerStore()")

    STATICINLINE void DisableProfiler()
    {
    }

    C_DEPRECATED_S("use State::GetProfilerStore()->flags instead")

    STATICINLINE void SetDeepProfileMode(bool)
    {
    }

    C_DEPRECATED_S("use ExtraData::Add() instead")

    STATICINLINE void AddExtraData(std::string const&, std::string_view const&)
    {
    }

    C_DEPRECATED_S("use ExtraData::Get() instead")

    STATICINLINE PExtraData* ExtraInfo()
    {
        return &Context().extra_data;
    }

    STATICINLINE i64 StartTime()
    {
        return Context().start_time.time_since_epoch().count();
    }

  private:
    STATICINLINE ThreadState& ThreadContext()
    {
        auto state = PContext::ProfilerTStore();

        C_PTR_CHECK(state);

        return *state;
    }

    STATICINLINE PContext& Context()
    {
        auto context = PContext::ProfilerStore();

        C_PTR_CHECK(context);

        return *context;
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
        std::string_view name = stl_types::source_location().function_name(),
        DataPoint::Attr  at   = DataPoint::AttrNone)
    {
        SimpleProfilerImpl::PushContext(name, at);
    }

    FORCEDINLINE ~ProfilerContext()
    {
        SimpleProfilerImpl::PopContext();
    }
};

struct DeepProfilerContext
{
    FORCEDINLINE DeepProfilerContext(
        std::string_view name = stl_types::source_location().function_name(),
        DataPoint::Attr  at   = DataPoint::AttrNone)
    {
        SimpleProfilerImpl::DeepPushContext(name, at);
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
        ThreadId::Hash   gpu_thread = 0x8085)
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
        auto props = Profiler::runtime_options::get_properties();

        Profiler::datapoint event;
        event.flags.type =
            offset ? Profiler::datapoint::Pop : Profiler::datapoint::Push;
        event.ts =
            (m_start + std::chrono::nanoseconds(offset)).time_since_epoch();
        event.name      = name;
        event.tid       = m_thread;
        event.component = COFFEE_COMPONENT_NAME;

        props.push(*props.context, event);
    }

    ThreadId::Hash             m_thread;
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
