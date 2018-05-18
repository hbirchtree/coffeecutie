#pragma once

#include "../../coffee_mem_macros.h"
#include "../../types/tdef/integertypes.h"

#include <thread>
#include <functional>

namespace Coffee{
#if defined(COFFEE_NO_THREADLIB)
namespace CurrentThread{
extern u32 get_id();
}
#endif
namespace Threads{

template<typename thread_t>
struct ThreadId_t
{
#if defined(COFFEE_NO_THREADLIB)
    using thread_id_t = u32;
#else
    using thread_id_t = typename thread_t::id;
#endif
    using Hash = uint64;

    FORCEDINLINE ThreadId_t():
#if defined(COFFEE_NO_THREADLIB)
        m_id(CurrentThread::get_id())
#else
        m_id(std::this_thread::get_id())
#endif
    {
    }
    FORCEDINLINE ThreadId_t(thread_id_t const& tid):
        m_id(tid)
    {
    }

    FORCEDINLINE bool operator==(const ThreadId_t& thd) const
    {
        return m_id == thd.m_id;
    }
    FORCEDINLINE Hash hash() const
    {
#if defined(COFFEE_GEKKO)
        return m_id;
#else
        return std::hash<thread_id_t>()(m_id);
#endif
    }
private:
    thread_id_t m_id;
};

}

}
