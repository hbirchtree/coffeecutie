#pragma once

#include <future>
#include <functional>

namespace Coffee{
namespace Threads{

template<typename RType>
using Future = std::future<RType>;
template<typename RType>
using SharedFuture = std::shared_future<RType>;

template<typename FunSignature>
using Function = std::function<FunSignature>;

using Thread = std::thread;

}
}