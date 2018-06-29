#pragma once

#include "../../plat/plat_quirks_toggling.h"

#include <functional>
#include <future>

#include "../../base/threading/thread_id.h"
#include "stltypes.h"

#if defined(COFFEE_ANDROID) && (defined(COFFEE_NO_PTHREAD_GETNAME_NP) || \
                                defined(COFFEE_NO_PTHREAD_SETNAME_NP))
#include <sys/prctl.h>
#endif

#if defined(COFFEE_GEKKO)
#include <gccore.h>
#include <ogcsys.h>
#include <sys/timespec.h>
#endif

namespace Coffee {

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
    void get()
    {
    }
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

#if defined(COFFEE_NO_THREADLIB)
struct Thread
{
    using id = u32;

    Thread(std::function<void()> f);
    template<
        typename TFun,
        typename std::enable_if<std::is_pointer<TFun>::value, bool>::type* =
            nullptr,
        typename... Args>
    Thread(TFun fptr, Args... args) : Thread([&]() { fptr(args...); })
    {
    }
    Thread();
    static inline i32 hardware_concurrency()
    {
        return 1;
    }
    void detach();
    void join();
    id   get_id();

  private:
#if defined(COFFEE_GEKKO)
    lwp_t                 m_threadHandle;
    std::function<void()> m_threadLambda;
#endif
};
#else
using Thread       = std::thread;
#endif

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
    CString cpy = name;
    if(name.size() >= 16)
        cpy.resize(15);
#if defined(COFFEE_APPLE)
    pthread_setname_np(cpy.c_str());
    return true;
#elif defined(COFFEE_UNIXPLAT) && !defined(COFFEE_NO_PTHREAD_SETNAME_NP)
    pthread_setname_np(pthread_self(), cpy.c_str());
    return true;
#elif defined(COFFEE_WINDOWS)
    CWString wname(name.begin(), name.end());
    SetThreadDescription(GetCurrentThread(), wname.c_str());
    return true;
#else
    return false;
#endif
}
FORCEDINLINE CString ThreadGetName(Thread& t)
{
    CString out;
    out.resize(17);
#if defined(COFFEE_UNIXPLAT) && !defined(COFFEE_NO_PTHREAD_GETNAME_NP)
    int stat = pthread_getname_np(t.native_handle(), &out[0], out.size());
    if(stat != 0)
        return out;
    out.resize(out.find('\0', 0));
    return out;
#else
    return out;
#endif
}
FORCEDINLINE CString ThreadGetName()
{
#if defined(COFFEE_UNIXPLAT) && !defined(COFFEE_NO_PTHREAD_GETNAME_NP)
    CString out;
    out.resize(17);
    int stat = pthread_getname_np(pthread_self(), &out[0], out.size());
    if(stat != 0)
        return out;
    out.resize(out.find('\0', 0));
    return out;
#elif defined(COFFEE_ANDROID)
    CString out;
    out.resize(17);
    int stat = prctl(PR_GET_NAME, &out[0], 0, 0, 0);
    out.resize(out.find('\0', 0));
    return out;
#else
    return {};
#endif
}

template<typename FunSignature>
using Function = std::function<FunSignature>;

using ThreadId = Threads::ThreadId_t<Thread>;

namespace CurrentThread {
#if defined(COFFEE_NO_THREADLIB)
template<typename Rep, typename Period>
STATICINLINE void sleep_for(const std::chrono::duration<Rep, Period>& dura)
{
#if defined(COFFEE_GEKKO)
    struct timespec sleepyTime;
    sleepyTime.tv_sec =
        std::chrono::duration_cast<std::chrono::seconds>(dura).count();
    sleepyTime.tv_nsec =
        std::chrono::duration_cast<std::chrono::nanoseconds>(dura).count() %
        1000000000;
    nanosleep(&sleepyTime);
#endif
}
template<typename Clock, typename Duration>
STATICINLINE void sleep_until(
    const std::chrono::time_point<Clock, Duration>& abs_time)
{
    sleep_for(abs_time - Clock::now());
}

extern void yield();

extern Thread::id get_id();

#else
using namespace std::this_thread;
#endif
}

namespace Chrono {
using namespace std::chrono;

using seconds_float  = duration<scalar>;
using seconds_double = duration<bigscalar>;

} // namespace Chrono
}
