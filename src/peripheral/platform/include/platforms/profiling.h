#pragma once

#include <peripherals/profiler/profiler.h>
#include <platforms/pimpl_state.h>

#ifndef COFFEE_COMPONENT_NAME
#define COFFEE_COMPONENT_NAME "(unknown)"
#endif

namespace platform {
namespace profiling {

using namespace ::libc_types;
using namespace ::stl_types;

#if defined(COFFEE_DISABLE_PROFILER)
using profiler_compile_opts = ::profiler::options::compile_default;
#else
using profiler_compile_opts = ::profiler::options::compile<true, true>;
#endif

using PClock     = Chrono::high_resolution_clock;
using PExtraData = Map<CString, CString>;

struct ThreadInternalState
{
    virtual ~ThreadInternalState();
};

struct ThreadState
{
    GlobalState*               writer;
    Vector<CString>            context_stack;
    ThreadId::Hash             thread_id;
    UqPtr<ThreadInternalState> internal_state;
};

struct PContext
{
    PContext() : start_time(PClock::now())
    {
        flags.enabled      = false;
        flags.deep_enabled = false;
    }

    PClock::time_point                      start_time;
    Mutex                                   access;
    Map<ThreadId::Hash, ShPtr<ThreadState>> thread_states;
    Map<CString, CString>                   extra_data;

    struct
    {
        atomic_bool enabled;
        atomic_bool deep_enabled;
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

    static ShPtr<PContext> ProfilerStore()
    {
        return state->GetProfilerStore();
    }

    static ShPtr<ThreadState> ProfilerTStore()
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

    ShPtr<ThreadState> context;

    void push_stack(CString const& frame)
    {
        PContext::ProfilerTStore()->context_stack.push_back(frame);
    }
    CString pop_stack()
    {
        auto& thread_store = *PContext::ProfilerTStore();

        if(!thread_store.context_stack.size())
            return {};

        CString out = thread_store.context_stack.back();
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
        UNUSED_PARAM(CString const&, k), UNUSED_PARAM(CString const&, v))
    {
        if constexpr(!compile_info::profiler::enabled)
            return;

        auto context = PContext::ProfilerStore();

        C_PTR_CHECK(context)

        Lock _(context->access);

        context->extra_data[k] = v;
    }

    STATICINLINE PExtraData Get()
    {
        if constexpr(!compile_info::profiler::enabled)
            return {};

        auto context = PContext::ProfilerStore();

        C_PTR_CHECK(context)

        return context->extra_data;
    }
};

#if MODE_DEBUG && !defined(COFFEE_DISABLE_PROFILER)
struct SimpleProfilerImpl
{
    STATICINLINE void PushContext(
        cstring name, DataPoint::Attr = DataPoint::AttrNone)
    {
        Profiler::app::push(name);
    }
    STATICINLINE void PopContext()
    {
        Profiler::app::pop();
    }
    STATICINLINE void Profile(
        cstring name, DataPoint::Attr = DataPoint::AttrNone)
    {
        Profiler::app::profile(name);
    }

    STATICINLINE void DeepPushContext(
        cstring name, DataPoint::Attr = DataPoint::AttrNone)
    {
        Profiler::lib::push(name);
    }
    STATICINLINE void DeepPopContext()
    {
        Profiler::lib::pop();
    }
    STATICINLINE void DeepProfile(
        cstring name, DataPoint::Attr = DataPoint::AttrNone)
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
    STATICINLINE void SetDeepProfileMode(bool state)
    {
    }

    C_DEPRECATED_S("use ExtraData::Add() instead")
    STATICINLINE void AddExtraData(CString const& key, CString const& value)
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
        cstring name, DataPoint::Attr = DataPoint::AttrNone)
    {
    }

    STATICINLINE void PopContext()
    {
    }

    STATICINLINE void Profile(
        cstring name, DataPoint::Attr = DataPoint::AttrNone)
    {
    }

    STATICINLINE void DeepPushContext(
        cstring name, DataPoint::Attr = DataPoint::AttrNone)
    {
    }

    STATICINLINE void DeepPopContext()
    {
    }

    STATICINLINE void DeepProfile(
        cstring name, DataPoint::Attr = DataPoint::AttrNone)
    {
    }

    C_DEPRECATED
    STATICINLINE void SetDeepProfileMode(bool state)
    {
    }

    C_DEPRECATED
    STATICINLINE void AddExtraData(CString const& key, CString const& value)
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
        cstring name, DataPoint::Attr at = DataPoint::AttrNone)
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
        cstring name, DataPoint::Attr at = DataPoint::AttrNone)
    {
        SimpleProfilerImpl::DeepPushContext(name, at);
    }

    FORCEDINLINE DeepProfilerContext(
        CString const& name, DataPoint::Attr at = DataPoint::AttrNone)
    {
        SimpleProfilerImpl::DeepPushContext(name.c_str(), at);
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
        cstring          name,
        ShPtr<QueryType> query,
        ThreadId::Hash   gpu_thread = 0x8085) :
        m_thread(gpu_thread),
        m_name(name), m_query(query)
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
        event.ts   = (m_start + Chrono::nanoseconds(offset)).time_since_epoch();
        event.name = name;
        event.tid  = m_thread;
        event.component = COFFEE_COMPONENT_NAME;

        props.push(*props.context, event);
    }

    ThreadId::Hash     m_thread;
    cstring            m_name;
    PClock::time_point m_start;
    ShPtr<QueryType>   m_query;
};

extern void               JsonPush(ThreadState& state, DataPoint const& data);
extern ShPtr<GlobalState> CreateJsonProfiler();

} // namespace profiling
} // namespace platform

#undef current_thread_id
#undef profiler_data_store
#undef profiler_tstore
#undef PFTRACE
