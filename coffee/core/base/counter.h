#ifndef COFFEE_DEBUG_CLASS_FRAMECOUNTER_H
#define COFFEE_DEBUG_CLASS_FRAMECOUNTER_H

#include "coffee/core/CTypes"

namespace Coffee{
namespace CFunctional{

typedef void (*CFrameCounterFun)(uint32,const void*);

struct CFrameCounter
{
    CFrameCounter(CFrameCounterFun fun):
        interval(0),
        m_timestamp(0),
        m_fun(fun),
        m_frames(0)
    {
    }

    uint32 interval;
    const void* ptr;
    void update(uint64 time);
    uint32 frames();
private:
    uint64 m_timestamp;
    CFrameCounterFun m_fun;
    uint32 m_frames;
};

}
}

#endif
