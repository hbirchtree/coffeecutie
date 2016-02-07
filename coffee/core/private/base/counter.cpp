#include <coffee/core/base/counter.h>

namespace Coffee{
namespace CFunctional{

void FrameCounter::update(uint64 time)
{
    m_frames++;
    if(time > m_timestamp)
    {
        m_timestamp = time+interval;
        this->m_fun(m_frames,ptr);
        m_frames = 0;
    }
}

uint32 FrameCounter::frames()
{
    return m_frames;
}

}
}
