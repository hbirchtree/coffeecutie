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
#define current_thread_id (&State::GetCurrentThreadId())

namespace Profiling{

struct DataPoint
{
    FORCEDINLINE DataPoint():
        thread()
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

    ThreadId thread;

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

struct ThreadData
{
    LinkList<CString> context_stack;
};

using ThreadStorage = Map<ThreadId::Hash, ThreadData>;
using ThreadListing = Map<ThreadId::Hash,CString>;
using ThreadItem = Pair<ThreadId::Hash,CString>;
using ThreadPtr = ShPtr<ThreadId>;
using ExtraData = LinkList<ExtraPair>;
using StackFrames = Set<CString>;

#if !defined(COFFEE_DISABLE_PROFILER)

/* Below variables have storage in extern_storage.cpp */

struct ProfilerDataStore
{
    Timestamp start_time;

    Mutex data_access_mutex;
#if !defined(NDEBUG)
    LinkList<DataPoint> datapoints;
    StackFrames stackframes;
    ThreadListing threadnames;
    ThreadStorage context_storage;
#endif

    ExtraData extra_data;

#if !defined(NDEBUG)
    bool Enabled;
    bool Deep_Profile;
#else
    static const constexpr bool Enabled = false;
    static const constexpr bool Deep_Profile = false;
#endif

    byte_t padding[2];

};

#if !defined(NDEBUG)
STATICINLINE LinkList<CString>& threadContextStack()
{
    auto hash = current_thread_id->hash();
    auto& ctxt = profiler_data_store->context_storage[hash];
    return ctxt.context_stack;
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

    STATICINLINE void DestroyProfiler()
    {
    }

    STATICINLINE void LabelThread(cstring name)
    {
#if !defined(COFFEE_DISABLE_PROFILER)
        ThreadSetName(name);
#if !defined(NDEBUG)
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

    STATICINLINE void Profile(cstring name = nullptr,
                              DataPoint::Attr at = DataPoint::AttrNone)
    {
#if !defined(COFFEE_DISABLE_PROFILER)
#if !defined(NDEBUG) && !defined(COFFEE_NO_TLS)
        if(!profiler_data_store)
            return;

        uint64 ts = Time::CurrentMicroTimestamp();

        Lock l(profiler_data_store->data_access_mutex);
        C_UNUSED(l);

        DataPoint p;
        p.tp = DataPoint::Profile;
        p.ts = ts;
        p.name = (name) ? name : threadContextStack().front();
        p.component = COFFEE_COMPONENT_NAME;
        p.at = at;

        profiler_data_store->datapoints.push_back(p);
#endif
#endif
    }

    STATICINLINE void PushContext(cstring name,
                                  DataPoint::Attr at = DataPoint::AttrNone)
    {
#if !defined(COFFEE_DISABLE_PROFILER)
#if !defined(NDEBUG) && !defined(COFFEE_NO_TLS)
        if(!profiler_data_store)
            return;

        Lock l(profiler_data_store->data_access_mutex);
        C_UNUSED(l);

        threadContextStack().push_front(name);

        DataPoint p;
        p.tp = DataPoint::Push;
        p.ts = 0;
        p.name = name;
        p.component = COFFEE_COMPONENT_NAME;
        p.at = at;

        profiler_data_store->datapoints.push_back(p);
        profiler_data_store->datapoints.back().ts = Time::CurrentMicroTimestamp();
#endif
#endif
    }
    STATICINLINE void PopContext()
    {
#if !defined(COFFEE_DISABLE_PROFILER)
#if !defined(NDEBUG) && !defined(COFFEE_NO_TLS)
        if(!profiler_data_store)
            return;

        uint64 ts = Time::CurrentMicroTimestamp();

        Lock l(profiler_data_store->data_access_mutex);
        C_UNUSED(l);

        if(threadContextStack().size()<1)
            return;

        DataPoint p;
        p.tp = DataPoint::Pop;
        p.ts = ts;
        p.name = threadContextStack().front();
        p.component = COFFEE_COMPONENT_NAME;

        profiler_data_store->datapoints.push_back(p);
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

    STATICINLINE void AddExtraData(CString const& key, CString const& val)
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

    STATICINLINE LinkList<DataPoint>* DataPoints()
    {
#if !defined(COFFEE_DISABLE_PROFILER)
#if !defined(NDEBUG)
        if(profiler_data_store)
            return &profiler_data_store->datapoints;
        else
#endif
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
    ThreadId::Hash th1 = t1.thread.hash();
    ThreadId::Hash th2 = t1.thread.hash();
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
