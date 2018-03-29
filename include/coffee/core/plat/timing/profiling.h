#pragma once

#include "timing_def.h"
#include "../../coffee_macros.h"
#include "../../base/threading/cthreading.h"
#include "../../internal_state.h"

#if defined(COFFEE_PROFILER_TRACING)
#include <coffee/core/CDebug>
#endif

#ifndef COFFEE_COMPONENT_NAME
#define COFFEE_COMPONENT_NAME "(unknown)"
#endif

namespace Coffee{

#if defined(COFFEE_PROFILER_TRACING)
#define PFTRACE(a) cVerbose(8, "PROFILER: {0}", a)
#else
#define PFTRACE(a)
#endif


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


/* Below variables have storage in extern_storage.cpp */

struct ProfilerDataStore
{
#if !defined(COFFEE_DISABLE_PROFILER)
    Timestamp start_time;

    Mutex data_access_mutex;
    ThreadListing threadnames;
    ThreadRefs thread_refs;

    ExtraData extra_data;

    bool Enabled;
    bool Deep_Profile;
    byte_t padding[2];
#endif // COFFEE_DISABLE_PROFILER
};

#if !defined(COFFEE_DISABLE_PROFILER)
STATICINLINE LinkList<CString>& threadContextStack()
{
    return profiler_tstore->context_stack;
}
#endif


struct SimpleProfilerImpl
{
    STATICINLINE void InitProfiler()
    {
#if !defined(COFFEE_DISABLE_PROFILER)
        profiler_data_store->Enabled = true;
        profiler_data_store->Deep_Profile = false;

        profiler_data_store->start_time =
                Time::CurrentMicroTimestamp();
#endif
    }

    STATICINLINE void DisableProfiler()
    {
#if !defined(COFFEE_DISABLE_PROFILER)
        profiler_data_store->Enabled = false;
        profiler_data_store->Deep_Profile = false;
#endif
    }

    STATICINLINE void LabelThread(cstring name)
    {
        ThreadSetName(name);

#if !defined(COFFEE_DISABLE_PROFILER)
        Lock _(profiler_data_store->data_access_mutex);
        ThreadId tid;
        profiler_data_store->threadnames
                .insert(ThreadItem(tid.hash(),name));
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
        if(!profiler_tstore)
            return;

        DataPoint p;
        p.tp = DataPoint::Profile;
        p.ts = Time::CurrentMicroTimestamp();
        p.name = (name) ? name : threadContextStack().front();
        p.component = COFFEE_COMPONENT_NAME;
        p.at = at;

        auto store = profiler_tstore;
        auto& points = store->datapoints;

        points.push_back(p);

        PFTRACE("PRF:" + p.name);
#endif
    }

    STATICINLINE void PushContext(
            cstring name, DataPoint::Attr at = DataPoint::AttrNone)
    {
#if !defined(COFFEE_DISABLE_PROFILER)
        if(!profiler_tstore)
            return;

        threadContextStack().push_front(name);

        DataPoint p;
        p.tp = DataPoint::Push;
        p.ts = Time::CurrentMicroTimestamp();
        p.name = name;
        p.component = COFFEE_COMPONENT_NAME;
        p.at = at;

        profiler_tstore->datapoints.push_back(p);
        profiler_tstore->datapoints.back().ts =
                Time::CurrentMicroTimestamp();

        PFTRACE("PSH:" + p.name);
#endif
    }
    STATICINLINE void PopContext()
    {
#if !defined(COFFEE_DISABLE_PROFILER)
        if(!profiler_tstore)
            return;

        if(threadContextStack().size()<1)
            throw implementation_error("invalid context pop");

        DataPoint p;
        p.tp = DataPoint::Pop;
        p.ts = Time::CurrentMicroTimestamp();
        p.name = threadContextStack().front();
        p.component = COFFEE_COMPONENT_NAME;

        profiler_tstore->datapoints.push_back(p);
        threadContextStack().pop_front();

        PFTRACE("POP:" + p.name);
#endif
    }

    STATICINLINE bool SetDeepProfileMode(bool state)
    {
#if !defined(COFFEE_DISABLE_PROFILER)
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
#if !defined(COFFEE_DISABLE_PROFILER)
        if(profiler_data_store && profiler_data_store->Deep_Profile)
        {
            Profile(name, at);
        }else
            return;
#endif
    }

    STATICINLINE void DeepPushContext(
            cstring name, DataPoint::Attr at = DataPoint::AttrNone)
    {
#if !defined(COFFEE_DISABLE_PROFILER)
        if(profiler_data_store
                && profiler_data_store->Deep_Profile)
        {
            PushContext(name, at);
        }else
            return;
#endif
    }

    STATICINLINE void DeepPopContext()
    {
#if !defined(COFFEE_DISABLE_PROFILER)
        if(profiler_data_store
                && profiler_data_store->Deep_Profile)
        {
            PopContext();
        }else
            return;
#endif
    }

    /*
     *
     * Accessors
     *
     */

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
        if(profiler_data_store)
            return profiler_data_store->Enabled;
        else
#endif
            return false;
    }

    STATICINLINE bool HasData()
    {
#if !defined(COFFEE_DISABLE_PROFILER)
        return profiler_data_store;
#else
        return false;
#endif
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
        if(profiler_data_store)
            return &profiler_data_store->threadnames;
        else
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
    DENYINLINE DeepProfilerContext(cstring name,
                        DataPoint::Attr at = DataPoint::AttrNone)
    {
        SimpleProfilerImpl::DeepPushContext(name, at);
    }
    DENYINLINE ~DeepProfilerContext()
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
#undef PFTRACE
