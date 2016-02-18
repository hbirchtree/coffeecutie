#ifndef COFFEE_CORE_PLAT_PROFILING_H
#define COFFEE_CORE_PLAT_PROFILING_H

#include "timing_def.h"
#include "../../coffee_macros.h"

namespace Coffee{

struct SimpleProfilerImpl
{
    struct DataPoint
    {
        enum Type
        {
            Push,
            Profile,
            Pop,
        };

        Type tp;
        CString name;
        uint64 ts;

        CString label;
        uint32 line;
    };

    STATICINLINE void InitProfiler()
    {
        data_access_mutex = new Mutex;
        datapoints = new std::list<DataPoint>;
        context_stack = new std::list<CString>;
    }

    STATICINLINE void DestroyProfiler()
    {
        delete data_access_mutex;
        delete datapoints;
        delete context_stack;
    }

    STATICINLINE void Profile(cstring name = nullptr)
    {
#ifndef NDEBUG
        DataPoint p;
        p.tp = DataPoint::Profile;
        p.ts = Time::CurrentMicroTimestamp();
        p.name = (name) ? name : context_stack->front();

        Lock l(*data_access_mutex);
        (void)&l;

        datapoints->push_back(p);
#endif
    }

    STATICINLINE void PushContext(CString const& name)
    {
#ifndef NDEBUG
        DataPoint p;
        p.tp = DataPoint::Push;
        p.ts = Time::CurrentMicroTimestamp();
        p.name = name;

        Lock l(*data_access_mutex);
        (void)&l;

        datapoints->push_back(p);
        context_stack->push_front(name);
#endif
    }
    STATICINLINE void PopContext()
    {
#ifndef NDEBUG
        DataPoint p;
        p.tp = DataPoint::Pop;
        p.ts = Time::CurrentMicroTimestamp();
        p.name = context_stack->front();

        Lock l(*data_access_mutex);
        (void)&l;

        datapoints->push_back(p);
        context_stack->pop_front();
#endif
    }

    thread_local static Mutex *data_access_mutex;
    thread_local static std::list<DataPoint> *datapoints;

protected:
    thread_local static std::list<CString> *context_stack;
};

using Profiler = SimpleProfilerImpl;

}

#endif