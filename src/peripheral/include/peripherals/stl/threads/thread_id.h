#pragma once

#include <peripherals/identify/identify.h>
#include <peripherals/libc/types.h>

#include <thread>

namespace stl_types {
namespace threads {

using namespace ::libc_types;

template<typename thread_t>
struct ThreadId_t
{
    using thread_id_t = typename thread_t::id;
    using Hash        = libc_types::u64;

    FORCEDINLINE ThreadId_t()
        : m_id(std::this_thread::get_id())
    {
    }

    FORCEDINLINE ThreadId_t(thread_id_t const& tid)
        : m_id(tid)
    {
    }

    FORCEDINLINE Hash operator()(thread_id_t h)
    {
        m_id = h;
        return hash();
    }

    FORCEDINLINE bool operator==(const ThreadId_t& thd) const
    {
        return m_id == thd.m_id;
    }

    FORCEDINLINE Hash hash() const
    {
        return std::hash<thread_id_t>()(m_id);
    }

  private:
    thread_id_t m_id;
};

} // namespace threads
} // namespace stl_types
