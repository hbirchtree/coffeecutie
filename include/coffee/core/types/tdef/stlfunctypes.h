#pragma once

#include <future>
#include <functional>

#include "../../base/threading/thread_id.h"

namespace Coffee{
namespace Threads{

template<typename RType>
using Future = std::future<RType>;
template<typename RType>
using SharedFuture = std::shared_future<RType>;


using Thread = std::thread;

}

template<typename FunSignature>
using Function = std::function<FunSignature>;

using ThreadId = Threads::ThreadId_t;

}
