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
    using id = libc_types::u32;

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
    static inline libc_types::i32 hardware_concurrency()
    {
        return 1;
    }
    void detach();
    void join();
    id   get_id();

  private:
#if defined(COFFEE_GEKKO)
    unsigned int          m_threadHandle;
    std::function<void()> m_threadLambda;
#endif
};
#else
using Thread       = std::thread;
#endif

using ThreadId = threads::ThreadId_t<Thread>;

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
    nanosleep(&sleepyTime, nullptr);
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

extern bool    SetName(CString const& name);
extern std::string_view GetName();
} // namespace CurrentThread

namespace Threads {
extern bool SetName(Thread& t, CString const& name);
extern bool SetName(ThreadId::Hash const& t, CString const& name);

extern std::string_view GetName(Thread& t);
extern std::string_view GetName(ThreadId::Hash t);

extern Map<ThreadId::Hash, CString> GetNames(
    platform::GlobalState* context = nullptr);
} // namespace Threads

FORCEDINLINE bool ThreadSetName(Thread& t, CString const& name)
{
    return Threads::SetName(t, name);
}
FORCEDINLINE std::string_view ThreadGetName(Thread& t)
{
    return Threads::GetName(t);
}

FORCEDINLINE bool ThreadSetName(ThreadId::Hash t, CString const& name)
{
    return Threads::SetName(t, name);
}
FORCEDINLINE std::string_view ThreadGetName(ThreadId::Hash t)
{
    return Threads::GetName(t);
}

/*!
 * \brief single-fire conditional
 */
struct concurrent_notif
{
    Mutex   awaiter;
    CondVar variable;

    Mutex prep_lock;

    FORCEDINLINE Mutex& prepare_lock()
    {
        prep_lock.lock();
        return prep_lock;
    }

    FORCEDINLINE void await()
    {
        UqLock lock(awaiter);

        prep_lock.unlock();

        variable.wait(lock);
    }

    template<typename Dur>
    FORCEDINLINE cv_status await(Dur const& dur)
    {
        UqLock lock(awaiter);

        prep_lock.unlock();

        return variable.wait_for(lock, dur);
    }

    FORCEDINLINE void notify()
    {
        (void)notify(Chrono::seconds(1));
    }

    template<typename Dur = Chrono::system_clock::duration>
    NO_DISCARD FORCEDINLINE bool notify(Dur const&)
    {
        UqLock _(prep_lock, std::try_to_lock);

        if(!_.owns_lock())
            return false;

        variable.notify_all();
        return true;
    }
};
} // namespace stl_types
