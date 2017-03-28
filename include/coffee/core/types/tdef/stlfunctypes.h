#pragma once

#include <future>
#include <functional>

#include "../../base/threading/thread_id.h"

namespace Coffee{
namespace Threads{

#if defined(COFFEE_NO_FUTURES)
template<typename RType>
struct Future
{
    RType get()
    {
        return retval;
    }

    RType retval;
};
template<>
struct Future<void>
{
    void get() {}
};
#else
template<typename RType>
using Future = std::future<RType>;
#endif

template<typename RType>
using SharedFuture = std::shared_future<RType>;

using Thread = std::thread;

}

template<typename FunSignature>
using Function = std::function<FunSignature>;

using ThreadId = Threads::ThreadId_t;

}
