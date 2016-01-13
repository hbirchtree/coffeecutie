#ifndef COFFEE_DEBUG_CLASS_FRAMECOUNTER_H
#define COFFEE_DEBUG_CLASS_FRAMECOUNTER_H

#include "coffee/core/CTypes"

namespace Coffee{
namespace CFunctional{

typedef void (*CFrameCounterFun)(uint32,const void*);

struct CFrameCounter
{
private:
    uint64 m_timestamp;
    CFrameCounterFun m_fun;
public:
    const void* ptr;
    uint32 interval;

    CFrameCounter(CFrameCounterFun fun):
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
};

}
}

#endif
