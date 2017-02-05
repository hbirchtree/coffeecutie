#ifndef COFFEE_CORE_PLAT_PROFILING_H
#define COFFEE_CORE_PLAT_PROFILING_H

#include "timing_def.h"
#include "../../coffee_macros.h"
#include "../../base/threading/cthreading.h"
//#include "../../CDebug"

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

        ThreadId thread;

        CString name;
        Timestamp ts;

        CString label;
        uint32 line;

        Type tp;
    };

    struct ExtraPair
    {
        CString key;
        CString value;
    };

    using ThreadListing = Map<ThreadId::Hash,CString>;
    using ThreadItem = Pair<ThreadId::Hash,CString>;
    using ThreadPtr = ShPtr<ThreadId>;
    using ExtraData = LinkList<ExtraPair>;

    STATICINLINE void ResetPointers()
    {
        /* Some platforms can't keep their pants on.
         * And they shit the bed.
         */
        profiler_data_store = nullptr;
    }

    STATICINLINE void InitProfiler()
    {
        if(!profiler_data_store)
        {
            profiler_data_store = new ProfilerDataStore;
            profiler_data_store->global_init.store(0);
        }
#if !defined(NDEBUG) && !defined(__EMSCRIPTEN__)
//        cVerbose(6,"Creating thread context stack");
        context_stack = new LinkList<CString>;
#endif

//        cVerbose(6,"Checking initializer value: {0}",global_init->load());
        if(profiler_data_store->global_init.load()<1)
        {
//            cVerbose(6,"Creating profiler");

            profiler_data_store->start_time = Time::CurrentMicroTimestamp();
        }

        profiler_data_store->global_init.fetch_add(1);
    }

    STATICINLINE void DestroyProfiler()
    {
#if !defined(NDEBUG) && !defined(__EMSCRIPTEN__)
        if(context_stack)
        {
            delete context_stack;
            context_stack = nullptr;
        }
#endif
        if(profiler_data_store &&
                std::atomic_fetch_sub(&profiler_data_store->global_init,1)<2)
        {
            delete profiler_data_store;
        }
    }

    STATICINLINE void LabelThread(cstring name)
    {
#if defined(COFFEE_UNIXPLAT) && !defined(__EMSCRIPTEN__)
        pthread_setname_np(pthread_self(), name);
#endif
#if !defined(NDEBUG) && !defined(__EMSCRIPTEN__)
        ThreadId tid;
        profiler_data_store->threadnames.insert(ThreadItem(tid.hash(),name));
#endif
    }

    /*
     * To remove the overhead of container operations from the measurements, timestamps
     *  are captured as early or as late as possible, depending on which is appropriate.
     * Linked lists have little insertion time, but we want to be sure.
     *
     */

    STATICINLINE void Profile(cstring name = nullptr)
    {
#if !defined(NDEBUG) && !defined(__EMSCRIPTEN__)
        uint64 ts = Time::CurrentMicroTimestamp();

        Lock l(profiler_data_store->data_access_mutex);
        C_UNUSED(l);

        if(!context_stack)
            return;

        DataPoint p;
        p.tp = DataPoint::Profile;
        p.ts = ts;
        p.name = (name) ? name : context_stack->front();

        profiler_data_store->datapoints.push_back(p);
#endif
    }

    STATICINLINE void PushContext(cstring name)
    {
#if !defined(NDEBUG) && !defined(__EMSCRIPTEN__)

        Lock l(profiler_data_store->data_access_mutex);
        C_UNUSED(l);

        if(!context_stack)
            return;

        context_stack->push_front(name);

        DataPoint p;
        p.tp = DataPoint::Push;
        p.ts = 0;
        p.name = name;

        profiler_data_store->datapoints.push_back(p);
        profiler_data_store->datapoints.back().ts = Time::CurrentMicroTimestamp();
#endif
    }
    STATICINLINE void PopContext()
    {
#if !defined(NDEBUG) && !defined(__EMSCRIPTEN__)
        uint64 ts = Time::CurrentMicroTimestamp();

        Lock l(profiler_data_store->data_access_mutex);
        C_UNUSED(l);

        if(!context_stack || context_stack->size()<1)
            return;

        DataPoint p;
        p.tp = DataPoint::Pop;
        p.ts = ts;
        p.name = context_stack->front();

        profiler_data_store->datapoints.push_back(p);
        context_stack->pop_front();
#endif
    }

    STATICINLINE void AddExtraData(CString const& key, CString const& val)
    {
        Lock l(profiler_data_store->data_access_mutex);
        C_UNUSED(l);

        profiler_data_store->extra_data.push_back({key,val});
    }


    /* Below variables have storage in extern_storage.cpp */

    struct ProfilerDataStore
    {
        Timestamp start_time;

        Mutex data_access_mutex;
    #if !defined(NDEBUG) && !defined(__EMSCRIPTEN__)
        LinkList<DataPoint> datapoints;
        ThreadListing threadnames;
    #endif

        ExtraData extra_data;
        std::atomic_int global_init;

#if !defined(NDEBUG) && !defined(__EMSCRIPTEN__)
        bool Enabled;
#else
        static const constexpr bool Enabled = false;
#endif
        byte_t padding[3];

    };

    static ProfilerDataStore* profiler_data_store;

#if !defined(NDEBUG) && !defined(__EMSCRIPTEN__)
    thread_local static LinkList<CString> *context_stack;
#endif

    STATICINLINE ExtraData* ExtraInfo()
    {
        if(profiler_data_store)
            return &profiler_data_store->extra_data;
        else
            return nullptr;
    }

    STATICINLINE LinkList<DataPoint>* DataPoints()
    {
#if !defined(NDEBUG) && !defined(__EMSCRIPTEN__)
        if(profiler_data_store)
            return &profiler_data_store->datapoints;
        else
#endif
            return nullptr;
    }

    STATICINLINE bool Enabled()
    {
#if !defined(NDEBUG) && !defined(__EMSCRIPTEN__)
        if(profiler_data_store)
            return profiler_data_store->Enabled;
        else
#endif
            return false;
    }

    STATICINLINE Timestamp StartTime()
    {
        if(profiler_data_store)
            return profiler_data_store->start_time;
        else
            return 0;
    }

    STATICINLINE ThreadListing* ThreadNames()
    {
#if !defined(NDEBUG) && !defined(__EMSCRIPTEN__)
        if(profiler_data_store)
            return &profiler_data_store->threadnames;
        else
#endif
            return nullptr;
    }
};

FORCEDINLINE bool operator<(SimpleProfilerImpl::DataPoint const& t1, SimpleProfilerImpl::DataPoint const& t2)
{
    ThreadId::Hash th1 = t1.thread.hash();
    ThreadId::Hash th2 = t1.thread.hash();
    bool thread_sort = th1<th2;
    bool samethread = th1==th2;
    bool ts_sort = t1.ts<t2.ts;
    return (samethread) ? ts_sort : thread_sort;
}

class SimpleProfilerContext
{
public:
    SimpleProfilerContext(cstring name)
    {
        SimpleProfilerImpl::PushContext(name);
    }
    ~SimpleProfilerContext()
    {
        SimpleProfilerImpl::PopContext();
    }
};

}

using ProfContext = Profiling::SimpleProfilerContext;
using Profiler = Profiling::SimpleProfilerImpl;

}

#endif
