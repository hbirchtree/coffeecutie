#pragma once

#include "timing_def.h"
#include "../../coffee_macros.h"
#include "../../base/threading/cthreading.h"
#include "../../internal_state.h"

#ifndef COFFEE_COMPONENT_NAME
#define COFFEE_COMPONENT_NAME "(unknown)"
#endif

namespace Coffee{

#define profiler_data_store (State::GetProfilerStore())
#define profiler_tstore (State::GetProfilerTStore())
#define current_thread_id (&State::GetCurrentThreadId())

namespace Profiling{

struct DataPoint
{
    FORCEDINLINE DataPoint():
        thread(ThreadId().hash())
    {
    }

    enum Type
    {
        Push,
        Profile,
        Pop,
    };

    enum Attr
    {
        AttrNone,
        Hot,
    };

    ThreadId::Hash thread;

    CString name;
    Timestamp ts;

    CString component;

//        CString label;
//        uint32 line;

    Type tp;
    Attr at;
};

struct ExtraPair
{
    CString key;
    CString value;
};

using ThreadListing = Map<ThreadId::Hash,CString>;
using ThreadItem = Pair<ThreadId::Hash,CString>;
using ExtraData = LinkList<ExtraPair>;
using StackFrames = Set<CString>;

struct ThreadData
{
    Vector<DataPoint> datapoints;
    LinkList<CString> context_stack;
};

using ThreadRefs = Map<ThreadId::Hash, ShPtr<ThreadData>>;

#if !defined(COFFEE_DISABLE_PROFILER)

/* Below variables have storage in extern_storage.cpp */

struct ProfilerDataStore
{
    Timestamp start_time;

    Mutex data_access_mutex;
#if !defined(NDEBUG)
    ThreadListing threadnames;
    ThreadRefs thread_refs;
#endif

    ExtraData extra_data;

#if !defined(NDEBUG)
    bool Enabled;
    bool Deep_Profile;
    byte_t padding[2];
#else
    static const constexpr bool Enabled = false;
    static const constexpr bool Deep_Profile = false;
#endif
};

#if !defined(NDEBUG) && !defined(COFFEE_DISABLE_PROFILER)
STATICINLINE LinkList<CString>& threadContextStack()
{
    return profiler_tstore->context_stack;
}
#endif

#endif // COFFEE_DISABLE_PROFILER

struct SimpleProfilerImpl
{
    STATICINLINE void InitProfiler()
    {
#if !defined(COFFEE_DISABLE_PROFILER)
#if !defined(NDEBUG)
        profiler_data_store->Enabled = true;
        profiler_data_store->Deep_Profile = false;
#endif

        profiler_data_store->start_time =
                Time::CurrentMicroTimestamp();
#endif
    }

    STATICINLINE void DisableProfiler()
    {
#if !defined(COFFEE_DISABLE_PROFILER) && !defined(NDEBUG)
        profiler_data_store->Enabled = false;
        profiler_data_store->Deep_Profile = false;
#endif
    }

    STATICINLINE void LabelThread(cstring name)
    {
#if !defined(COFFEE_DISABLE_PROFILER)
        ThreadSetName(name);
#if !defined(NDEBUG)

        Lock _(profiler_data_store->data_access_mutex);
        ThreadId tid;
        profiler_data_store->threadnames
                .insert(ThreadItem(tid.hash(),name));

#endif
#endif
    }

    /*
     * To remove the overhead of container operations from the measurements, timestamps
     *  are captured as early or as late as possible, depending on which is appropriate.
     * Linked lists have little insertion time, but we want to be sure.
     *
     */

    STATICINLINE void Profile(
            cstring name = nullptr,
            DataPoint::Attr at = DataPoint::AttrNone)
    {
#if !defined(COFFEE_DISABLE_PROFILER)
#if !defined(NDEBUG) && !defined(COFFEE_NO_TLS)
        if(!profiler_tstore)
            return;

        uint64 ts = Time::CurrentMicroTimestamp();

        DataPoint p;
        p.tp = DataPoint::Profile;
        p.ts = ts;
        p.name = (name) ? name : threadContextStack().front();
        p.component = COFFEE_COMPONENT_NAME;
        p.at = at;

        auto store = profiler_tstore;
        auto& points = store->datapoints;

        points.push_back(p);
#endif
#endif
    }

    STATICINLINE void PushContext(
            cstring name, DataPoint::Attr at = DataPoint::AttrNone)
    {
#if !defined(COFFEE_DISABLE_PROFILER)
#if !defined(NDEBUG) && !defined(COFFEE_NO_TLS)
        if(!profiler_tstore)
            return;

        threadContextStack().push_front(name);

        DataPoint p;
        p.tp = DataPoint::Push;
        p.ts = 0;
        p.name = name;
        p.component = COFFEE_COMPONENT_NAME;
        p.at = at;

        profiler_tstore->datapoints.push_back(p);
        profiler_tstore->datapoints.back().ts =
                Time::CurrentMicroTimestamp();
#endif
#endif
    }
    STATICINLINE void PopContext()
    {
#if !defined(COFFEE_DISABLE_PROFILER)
#if !defined(NDEBUG) && !defined(COFFEE_NO_TLS)
        if(!profiler_tstore)
            return;

        uint64 ts = Time::CurrentMicroTimestamp();

        if(threadContextStack().size()<1)
            return;

        DataPoint p;
        p.tp = DataPoint::Pop;
        p.ts = ts;
        p.name = threadContextStack().front();
        p.component = COFFEE_COMPONENT_NAME;

        profiler_tstore->datapoints.push_back(p);
        threadContextStack().pop_front();
#endif
#endif
    }

    STATICINLINE bool SetDeepProfileMode(bool state)
    {
#if !defined(COFFEE_DISABLE_PROFILER) && !defined(NDEBUG)
        if(!profiler_data_store)
            return false;

        profiler_data_store->Deep_Profile = state;

        return true;
#else
        return false;
#endif
    }

    STATICINLINE void DeepProfile(
            cstring name, DataPoint::Attr at = DataPoint::AttrNone)
    {
#if !defined(COFFEE_DISABLE_PROFILER) && !defined(NDEBUG)
        if(profiler_data_store
                && profiler_data_store->Deep_Profile)
        {
            Profile(name, at);
        }
#endif
    }

    STATICINLINE void DeepPushContext(
            cstring name, DataPoint::Attr at = DataPoint::AttrNone)
    {
#if !defined(COFFEE_DISABLE_PROFILER) && !defined(NDEBUG)
        if(profiler_data_store
                && profiler_data_store->Deep_Profile)
        {
            PushContext(name, at);
        }
#endif
    }

    STATICINLINE void DeepPopContext()
    {
#if !defined(COFFEE_DISABLE_PROFILER) && !defined(NDEBUG)
        if(profiler_data_store
                && profiler_data_store->Deep_Profile)
        {
            PopContext();
        }
#endif
    }

    STATICINLINE void AddExtraData(
            CString const& key, CString const& val)
    {
#if !defined(COFFEE_DISABLE_PROFILER)
        if(!profiler_data_store)
            return;

        Lock l(profiler_data_store->data_access_mutex);
        C_UNUSED(l);

        profiler_data_store->extra_data.push_back({key,val});
#endif
    }

    STATICINLINE ExtraData* ExtraInfo()
    {
#if !defined(COFFEE_DISABLE_PROFILER)
        if(profiler_data_store)
            return &profiler_data_store->extra_data;
        else
#endif
            return nullptr;
    }

    STATICINLINE bool Enabled()
    {
#if !defined(COFFEE_DISABLE_PROFILER)
#if !defined(NDEBUG)
        if(profiler_data_store)
            return profiler_data_store->Enabled;
        else
#endif
#endif
            return false;
    }

    STATICINLINE Timestamp StartTime()
    {
#if !defined(COFFEE_DISABLE_PROFILER)
        if(profiler_data_store)
            return profiler_data_store->start_time;
        else
#endif
            return 0;
    }

    STATICINLINE ThreadListing* ThreadNames()
    {
#if !defined(COFFEE_DISABLE_PROFILER)
#if !defined(NDEBUG)
        if(profiler_data_store)
            return &profiler_data_store->threadnames;
        else
#endif
#endif
            return nullptr;
    }
};

FORCEDINLINE bool operator<(DataPoint const& t1,
                            DataPoint const& t2)
{
    ThreadId::Hash th1 = t1.thread;
    ThreadId::Hash th2 = t1.thread;
    bool thread_sort = th1<th2;
    bool samethread = th1==th2;
    bool ts_sort = t1.ts<t2.ts;
    return (samethread) ? ts_sort : thread_sort;
}

struct SimpleProfilerContext
{
    SimpleProfilerContext(cstring name,
                          DataPoint::Attr at = DataPoint::AttrNone)
    {
        SimpleProfilerImpl::PushContext(name, at);
    }
    ~SimpleProfilerContext()
    {
        SimpleProfilerImpl::PopContext();
    }
};

struct DeepProfilerContext
{
    DeepProfilerContext(cstring name,
                        DataPoint::Attr at = DataPoint::AttrNone)
    {
        SimpleProfilerImpl::DeepPushContext(name, at);
    }
    ~DeepProfilerContext()
    {
        SimpleProfilerImpl::DeepPopContext();
    }
};

}

using DProfContext = Profiling::DeepProfilerContext;
using ProfContext = Profiling::SimpleProfilerContext;
using Profiler = Profiling::SimpleProfilerImpl;

}

#undef current_thread_id
#undef profiler_data_store
#undef profiler_tstore
