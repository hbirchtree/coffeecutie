#ifndef COFFEE_THREADING
#define COFFEE_THREADING

#include <thread>
#include <functional>
#include <future>

#include "thread_id.h"
#include "async_tasks.h"
#include "job_system.h"

namespace Coffee{
namespace Threads{

template<typename T>
using Future = std::future<T>;

}

using ThreadId = Threads::ThreadId_t;

}

#endif //COFFEE_THREADING
