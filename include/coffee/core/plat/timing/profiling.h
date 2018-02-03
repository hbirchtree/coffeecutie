#pragma once

#include "timing_def.h"
#include "../../coffee_macros.h"
#include "../../base/threading/cthreading.h"

#ifndef COFFEE_COMPONENT_NAME
#define COFFEE_COMPONENT_NAME "(unknown)"
#endif

namespace Coffee{
namespace Profiling{

struct SimpleProfilerImpl
{
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

    STATICINLINE void ResetPointers()
    {
        /* Some platforms can't keep their pants on.
         * And they shit the bed.
         */
#if !defined(COFFEE_DISABLE_PROFILER)
        profiler_data_store = nullptr;
#endif
    }

    STATICINLINE void InitProfiler()
    {
#if !defined(COFFEE_DISABLE_PROFILER)
        if(!profiler_data_store)
        {
            profiler_data_store = new ProfilerDataStore;
            profiler_data_store->global_init.store(0);
            profiler_data_store->Enabled = true;
            profiler_data_store->Deep_Profile = false;
        }

        if(profiler_data_store->global_init.load()<1)
        {
            profiler_data_store->start_time =
                    Time::CurrentMicroTimestamp();
        }

        profiler_data_store->global_init.fetch_add(1);
#endif
    }

    STATICINLINE void DestroyProfiler()
    {
#if !defined(COFFEE_DISABLE_PROFILER)
        if(profiler_data_store &&
                std::atomic_fetch_sub(&profiler_data_store->global_init,1)<2)
        {
            delete profiler_data_store;
            profiler_data_store = nullptr;
        }
#endif
    }

    STATICINLINE void LabelThread(cstring name)
    {
#if !defined(COFFEE_DISABLE_PROFILER)
        ThreadSetName(name);
#if !defined(NDEBUG)
        ThreadId tid;
        profiler_data_store->threadnames.insert(ThreadItem(tid.hash(),name));
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
        AtomicInt32 global_init;

#if !defined(NDEBUG)
        bool Enabled;
        bool Deep_Profile;
#else
        static const constexpr bool Enabled = false;
        static const constexpr bool Deep_Profile = false;
#endif
        byte_t padding[6];

    };

    static ProfilerDataStore* profiler_data_store;

#if !defined(NDEBUG) && !defined(COFFEE_NO_TLS)
    /* Because it's thread_local, it is initialized automatically */
    thread_local static ThreadId current_thread_id;
#else
    static ThreadId current_thread_id;
#endif


    STATICINLINE LinkList<CString>& threadContextStack()
    {
        auto hash = current_thread_id.hash();
        auto& ctxt = profiler_data_store->context_storage[hash];
        return ctxt.context_stack;
    }
#endif

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

FORCEDINLINE bool operator<(SimpleProfilerImpl::DataPoint const& t1,
                            SimpleProfilerImpl::DataPoint const& t2)
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
    using DataPoint = SimpleProfilerImpl::DataPoint;

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
    using DataPoint = SimpleProfilerImpl::DataPoint;

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
