#pragma once

#include <future>
#include <functional>

#include "stltypes.h"
#include "../../base/threading/thread_id.h"

namespace Coffee{

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
enum class FutureStatus
{
    ready,
    timeout,
    deferred
};
#else
template<typename RType>
using Future = std::future<RType>;

using FutureStatus = std::future_status;
#endif

template<typename RType>
using SharedFuture = std::shared_future<RType>;

using Thread = std::thread;

FORCEDINLINE bool ThreadSetName(Thread& t, CString const& name)
{
#if defined(COFFEE_APPLE)
//    pthread_setname_np(name.c_str());
    return false;
#elif defined(COFFEE_UNIXPLAT) && !defined(COFFEE_NO_PTHREAD_SETNAME_NP)
    pthread_setname_np(t.native_handle(), name.c_str());
    return true;
#else
    return false;
#endif
}
FORCEDINLINE bool ThreadSetName(CString const& name)
{
#if defined(COFFEE_APPLE)
    pthread_setname_np(name.c_str());
    return true;
#elif defined(COFFEE_UNIXPLAT) && !defined(COFFEE_NO_PTHREAD_SETNAME_NP)
    pthread_setname_np(pthread_self(), name.c_str());
    return true;
#else
    return false;
#endif
}
FORCEDINLINE CString ThreadGetName(Thread& t)
{
#if defined(COFFEE_UNIXPLAT) && !defined(COFFEE_NO_PTHREAD_SETNAME_NP)
    CString out;
    out.resize(32);
    int stat = pthread_getname_np(t.native_handle(), &out[0], out.size());
    if(stat != 0)
        return out;
    out.resize(out.find('\0', 0));
    return out;
#else
    return {};
#endif
}
FORCEDINLINE CString ThreadGetName()
{
#if defined(COFFEE_UNIXPLAT) && !defined(COFFEE_NO_PTHREAD_SETNAME_NP)
    CString out;
    out.resize(32);
    int stat = pthread_getname_np(pthread_self(), &out[0], out.size());
    if(stat != 0)
        return out;
    out.resize(out.find('\0', 0));
    return out;
#else
    return {};
#endif
}

template<typename FunSignature>
using Function = std::function<FunSignature>;

using ThreadId = Threads::ThreadId_t<std::thread>;

namespace CurrentThread{
using namespace std::this_thread;
}

namespace Chrono{
using namespace std::chrono;
}

}
