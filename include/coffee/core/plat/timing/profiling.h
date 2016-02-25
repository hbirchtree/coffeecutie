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

    static Mutex *data_access_mutex;
    static std::list<DataPoint> *datapoints;

protected:
    static std::list<CString> *context_stack;
};
}

using Profiler = Profiling::SimpleProfilerImpl;

}

#endif
