#pragma once

#include <thread>
#include "../../coffee_mem_macros.h"
#include "../../types/tdef/integertypes.h"

namespace Coffee{
namespace Threads{

struct ThreadId_t
{
    FORCEDINLINE ThreadId_t():
        m_id(std::this_thread::get_id())
    {
    }
    FORCEDINLINE bool operator==(const ThreadId_t& thd) const
    {
        return m_id == thd.m_id;
    }
    FORCEDINLINE uint64 hash() const
    {
        return std::hash<std::thread::id>()(m_id);
    }
private:
    const std::thread::id m_id;
};

}
}