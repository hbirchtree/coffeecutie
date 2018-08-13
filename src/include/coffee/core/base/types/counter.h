#ifndef COFFEE_DEBUG_CLASS_FRAMECOUNTER_H
#define COFFEE_DEBUG_CLASS_FRAMECOUNTER_H

#include "../../coffee_mem_macros.h"
#include "../../types/tdef/integertypes.h"

namespace Coffee{
namespace CFunctional{

struct counter_t
{
public:
    using CounterFun = void(*)(u32,const void*);

    const void* ptr;
    u32 interval;

    FORCEDINLINE counter_t(CounterFun fun, u32 interval = 1000):
        ptr(nullptr),
        interval(interval),
        m_timestamp(0),
        m_fun(fun),
        m_frames(0)
    {
    }
    FORCEDINLINE void update(u64 time)
    {
        m_frames++;
        if(time > m_timestamp)
        {
            m_timestamp = time+interval;
            this->m_fun(m_frames,ptr);
            m_frames = 0;
        }
    }
    FORCEDINLINE u32 frames()
    {
        return m_frames;
    }

private:
    u64 m_timestamp;
    CounterFun m_fun;
    u32 m_frames;
};

}

using Counter = CFunctional::counter_t;

}

#endif
