#pragma once

#include <coffee/core/base_state.h>
#include <peripherals/profiler/profiler.h>

#ifndef COFFEE_COMPONENT_NAME
#define COFFEE_COMPONENT_NAME "(unknown)"
#endif

namespace platform {
namespace profiling {

using namespace ::Coffee;
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
    State::GlobalState*        writer;
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
            State::GetProfilerTStore()->internal_state.get());
    }
    static bool enabled()
    {
        if(!State::ProfilerEnabled())
            return false;

        return State::GetProfilerStore()->flags.enabled;
    }
    static bool deep_enabled()
    {
        if(!enabled())
            return false;

        auto context = State::GetProfilerStore();
        Lock _(context->access);
        return context->flags.deep_enabled;
    }

    ShPtr<ThreadState> context;

    void push_stack(CString const& frame)
    {
        State::GetProfilerTStore()->context_stack.push_back(frame);
    }
    CString pop_stack()
    {
        auto& thread_store = *State::GetProfilerTStore();

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
    STATICINLINE void Add(CString const& k, CString const& v)
    {
        auto context = State::GetProfilerStore();

        C_PTR_CHECK(context);

        Lock _(context->access);

        context->extra_data[k] = v;
    }

    STATICINLINE PExtraData Get()
    {
        auto context = State::GetProfilerStore();

        C_PTR_CHECK(context);

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
        auto state = State::GetProfilerTStore();

        C_PTR_CHECK(state);

        return *state;
    }

    STATICINLINE PContext& Context()
    {
        auto context = State::GetProfilerStore();

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

extern void JsonPush(ThreadState& state, DataPoint const& data);
extern ShPtr<State::GlobalState> CreateJsonProfiler();

} // namespace profiling
} // namespace platform

#undef current_thread_id
#undef profiler_data_store
#undef profiler_tstore
#undef PFTRACE