#pragma once

#include <peripherals/stl/types.h>
#include <platforms/pimpl_state.h>

#include <thread>

namespace stl_types {

using thread_id_t = libc_types::u64;

inline thread_id_t get_this_thread_id()
{
    return std::hash<std::thread::id>()(std::this_thread::get_id());
}

inline thread_id_t get_thread_id(std::thread const& thread)
{
    return std::hash<std::thread::id>()(thread.get_id());
}

namespace CurrentThread {
using namespace std::this_thread;

extern bool             SetName(std::string const& name);
extern std::string_view GetName();
} // namespace CurrentThread

namespace Threads {

extern bool SetName(std::thread& t, std::string const& name);
extern bool SetName(thread_id_t const& t, std::string const& name);

extern std::string_view GetName(std::thread& t);
extern std::string_view GetName(thread_id_t t);

extern std::map<thread_id_t, std::string> GetNames(
    platform::GlobalState* context = nullptr);
} // namespace Threads
} // namespace stl_types
