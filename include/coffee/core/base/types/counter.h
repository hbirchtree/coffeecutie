#ifndef COFFEE_DEBUG_CLASS_FRAMECOUNTER_H
#define COFFEE_DEBUG_CLASS_FRAMECOUNTER_H

#include "../../coffee_mem_macros.h"
#include "../../types/tdef/integertypes.h"

namespace Coffee{
namespace CFunctional{

struct FrameCounter
{
public:
    using FrameCounterFun = void(*)(uint32,const void*);

    const void* ptr;
    uint32 interval;

    FORCEDINLINE FrameCounter(FrameCounterFun fun, uint32 interval = 1000):
        ptr(nullptr),
        interval(interval),
        m_timestamp(0),
        m_fun(fun),
        m_frames(0)
    {
    }
    FORCEDINLINE void update(uint64 time)
    {
        m_frames++;
        if(time > m_timestamp)
        {
            m_timestamp = time+interval;
            this->m_fun(m_frames,ptr);
            m_frames = 0;
        }
    }
    FORCEDINLINE uint32 frames()
    {
        return m_frames;
    }

private:
    uint32 m_frames;
    uint64 m_timestamp;
    FrameCounterFun m_fun;
};

}
}

#endif
