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

        Type tp;
        CString name;
        Timestamp ts;

        CString label;
        uint32 line;

        bool operator<(DataPoint const& t1)
        {
            return this->thread.hash()<t1.thread.hash();
        }
    };

    using ThreadListing = Map<ShPtr<ThreadId>,CString>;
    using ThreadItem = std::pair<ShPtr<ThreadId>,CString>;
    using ThreadPtr = ShPtr<ThreadId>;

    STATICINLINE void InitProfiler()
    {
#ifndef NDEBUG
        context_stack = new LinkList<CString>;

        if(!global_init)
        {
            global_init = new std::atomic_int(0);
        }

        if(global_init->load()<1)
        {
            start_time = new Timestamp(Time::CurrentMicroTimestamp());
            data_access_mutex = new Mutex;
            datapoints = new LinkList<DataPoint>;
            threadnames = new ThreadListing;
        }

        global_init->fetch_add(1);
#endif
    }

    STATICINLINE void DestroyProfiler()
    {
#ifndef NDEBUG
        if(context_stack)
        {
            delete context_stack;
            context_stack = nullptr;
        }
        if(global_init&&std::atomic_fetch_sub(global_init,1)<2)
        {
            delete data_access_mutex;
            delete datapoints;
            delete start_time;
            delete threadnames;
            delete global_init;
        }
#endif
    }

    STATICINLINE void LabelThread(cstring name)
    {
#ifndef NDEBUG
        threadnames->insert(ThreadItem(ThreadPtr(new ThreadId()),CString(name)));
#endif
    }

    STATICINLINE void Profile(cstring name = nullptr)
    {
#ifndef NDEBUG
        Lock l(*data_access_mutex);
        C_UNUSED(l);

        DataPoint p;
        p.tp = DataPoint::Profile;
        p.ts = Time::CurrentMicroTimestamp();
        p.name = (name) ? name : context_stack->front();

        datapoints->push_back(p);
#endif
    }

    STATICINLINE void PushContext(CString const& name)
    {
#ifndef NDEBUG
        Lock l(*data_access_mutex);
        C_UNUSED(l);

        DataPoint p;
        p.tp = DataPoint::Push;
        p.ts = Time::CurrentMicroTimestamp();
        p.name = name;

        datapoints->push_back(p);
        context_stack->push_front(name);
#endif
    }
    STATICINLINE void PopContext()
    {
#ifndef NDEBUG
        Lock l(*data_access_mutex);
        C_UNUSED(l);

        DataPoint p;
        p.tp = DataPoint::Pop;
        p.ts = Time::CurrentMicroTimestamp();
        p.name = context_stack->front();

        datapoints->push_back(p);
        context_stack->pop_front();
#endif
    }

    static Timestamp *start_time;
    static Mutex *data_access_mutex;
    static LinkList<DataPoint> *datapoints;
    static ThreadListing *threadnames;

protected:
    static std::atomic_int *global_init;
    thread_local static LinkList<CString> *context_stack;
};
}

using Profiler = Profiling::SimpleProfilerImpl;

}

#endif
