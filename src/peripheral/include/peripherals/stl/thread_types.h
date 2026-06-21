#pragma once

#include "threads/thread_id.h"
#include <peripherals/identify/identify.h>
#include <peripherals/stl/time_types.h>
#include <peripherals/stl/types.h>
#include <platforms/pimpl_state.h>

#include <functional>
#include <future>

#if defined(COFFEE_ANDROID) && (defined(COFFEE_NO_PTHREAD_GETNAME_NP) || \
                                defined(COFFEE_NO_PTHREAD_SETNAME_NP))
#include <sys/prctl.h>
#endif

namespace stl_types {
template<typename RType>
using Future = std::future<RType>;

using FutureStatus = std::future_status;

#include <thread>
using Thread = std::thread;

using ThreadId = threads::ThreadId_t<std::thread>;

namespace CurrentThread {
using namespace std::this_thread;

extern bool             SetName(std::string const& name);
extern std::string_view GetName();
} // namespace CurrentThread

namespace Threads {

extern bool SetName(std::thread& t, std::string const& name);
extern bool SetName(ThreadId::Hash const& t, std::string const& name);

extern std::string_view GetName(std::thread& t);
extern std::string_view GetName(ThreadId::Hash t);

extern std::map<ThreadId::Hash, std::string> GetNames(
    platform::GlobalState* context = nullptr);
} // namespace Threads
} // namespace stl_types
