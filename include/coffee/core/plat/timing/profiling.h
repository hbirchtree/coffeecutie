#ifndef COFFEE_CORE_PLAT_PROFILING_H
#define COFFEE_CORE_PLAT_PROFILING_H

#include "timing_def.h"
#include "../../coffee_macros.h"
#include "../../base/threading/cthreading.h"

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

    using ThreadListing = Map<ShPtr<ThreadId>,CString>;
    using ThreadItem = std::pair<ShPtr<ThreadId>,CString>;
    using ThreadPtr = ShPtr<ThreadId>;
    using ExtraData = LinkList<ExtraPair>;

    STATICINLINE void InitProfiler()
    {
        if(!global_init)
        {
            global_init = new std::atomic_int(0);
        }
#ifndef NDEBUG
        context_stack = new LinkList<CString>;
#endif


        if(global_init->load()<1)
        {
#ifndef NDEBUG
            datapoints = new LinkList<DataPoint>;
            threadnames = new ThreadListing;
#endif
            start_time = new Timestamp(Time::CurrentMicroTimestamp());
            data_access_mutex = new Mutex;
            extra_data = new ExtraData;
        }

        global_init->fetch_add(1);
    }

    STATICINLINE void DestroyProfiler()
    {
#ifndef NDEBUG
        if(context_stack)
        {
            delete context_stack;
            context_stack = nullptr;
        }
#endif
        if(global_init&&std::atomic_fetch_sub(global_init,1)<2)
        {
#ifndef NDEBUG
            delete datapoints;
            delete threadnames;
#endif
            delete data_access_mutex;
            delete start_time;
            delete global_init;
            delete extra_data;
        }
    }

    STATICINLINE void LabelThread(cstring name)
    {
#ifndef NDEBUG
        threadnames->insert(ThreadItem(ThreadPtr(new ThreadId()),CString(name)));
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
#ifndef NDEBUG
        uint64 ts = Time::CurrentMicroTimestamp();

        Lock l(*data_access_mutex);
        C_UNUSED(l);

        if(!context_stack)
            return;

        DataPoint p;
        p.tp = DataPoint::Profile;
        p.ts = ts;
        p.name = (name) ? name : context_stack->front();

        datapoints->push_back(p);
#endif
    }

    STATICINLINE void PushContext(cstring name)
    {
#ifndef NDEBUG

        Lock l(*data_access_mutex);
        C_UNUSED(l);

        if(!context_stack)
            return;

        context_stack->push_front(name);

        DataPoint p;
        p.tp = DataPoint::Push;
        p.ts = 0;
        p.name = name;

        datapoints->push_back(p);
        datapoints->back().ts = Time::CurrentMicroTimestamp();
#endif
    }
    STATICINLINE void PopContext()
    {
#ifndef NDEBUG
        uint64 ts = Time::CurrentMicroTimestamp();

        Lock l(*data_access_mutex);
        C_UNUSED(l);

        if(!context_stack || context_stack->size()<1)
            return;

        DataPoint p;
        p.tp = DataPoint::Pop;
        p.ts = ts;
        p.name = context_stack->front();

        datapoints->push_back(p);
        context_stack->pop_front();
#endif
    }

    STATICINLINE void AddExtraData(CString const& key, CString const& val)
    {
        Lock l(*data_access_mutex);
        C_UNUSED(l);

        extra_data->push_back({key,val});
    }

    /* Below variables have storage in extern_storage.cpp */

    static Timestamp *start_time;
    static Mutex *data_access_mutex;

#ifndef NDEBUG
    static LinkList<DataPoint> *datapoints;
    static ThreadListing *threadnames;
#endif

    static ExtraData *extra_data;

    static bool Enabled;

protected:
    static std::atomic_int *global_init;
#ifndef NDEBUG
    thread_local static LinkList<CString> *context_stack;
#endif
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
