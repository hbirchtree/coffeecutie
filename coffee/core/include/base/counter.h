#ifndef COFFEE_DEBUG_CLASS_FRAMECOUNTER_H
#define COFFEE_DEBUG_CLASS_FRAMECOUNTER_H

#include "coffee/core/CTypes"

namespace Coffee{
namespace CFunctional{

struct FrameCounter
{
public:
    using FrameCounterFun = void(*)(uint32,const void*);

    const void* ptr;
    uint32 interval;

    FrameCounter(FrameCounterFun fun):
        m_timestamp(0),
        m_fun(fun),
        interval(0),
        m_frames(0)
    {
    }
    void update(uint64 time);
    uint32 frames();

private:
    uint32 m_frames;
    uint64 m_timestamp;
    FrameCounterFun m_fun;
};

}
}

#endif
