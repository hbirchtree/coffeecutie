#pragma once

#include <peripherals/identify/system.h>

#include <atomic>
#include <condition_variable>
#include <mutex>

#if defined(COFFEE_GEKKO)
using mutex_handle_t = long unsigned int;
#endif

namespace stl_types {

#if defined(COFFEE_NO_THREADLIB)
struct Mutex
{
    Mutex();
    ~Mutex();
    void lock();
    bool try_lock();
    void unlock();

  private:
#if defined(COFFEE_GEKKO)
    unsigned int m_mutexHandle;
#endif
};

struct RecMutex
{
    RecMutex();
    ~RecMutex();
    void lock();
    bool try_lock();
    void unlock();

  private:
#if defined(COFFEE_GEKKO)
    unsigned int m_mutexHandle;
#endif
};

enum class cv_status
{
    no_timeout,
    timeout,
};

template<typename MutexType = Mutex>
struct BaseLock
{
    BaseLock(MutexType& m) : m_mutex(m)
    {
        m_mutex.lock();
    }
    ~BaseLock()
    {
        m_mutex.unlock();
    }

  private:
    MutexType& m_mutex;
};

using Lock    = BaseLock<>;
using RecLock = BaseLock<RecMutex>;

struct UqLock
{
    UqLock()
    {
    }
    UqLock(Mutex& m) : m_mutex(&m), m_hasLock(true)
    {
        m_mutex->try_lock();
    }
    UqLock(Mutex& m, std::try_to_lock_t) : m_mutex(&m)
    {
        m_hasLock = m_mutex->try_lock() == 0;
    }
    UqLock(Mutex& m, std::defer_lock_t) : m_mutex(&m), m_hasLock(false)
    {
    }
    ~UqLock()
    {
        m_mutex->unlock();
    }

    bool owns_lock() const
    {
        return m_hasLock;
    }

    Mutex* mutex()
    {
        return m_mutex;
    }

  private:
    Mutex* m_mutex;
    bool   m_hasLock;
};

struct CondVar
{
    CondVar();
    ~CondVar();

    void wait(UqLock& lock);

    template<typename Rep, typename Dur>
    cv_status wait_for(UqLock& lock, std::chrono::duration<Rep, Dur> const& dur)
    {
        return wait_limited(
            std::chrono::duration_cast<std::chrono::nanoseconds>(dur).count());
    }

    template<typename Rep, typename Dur>
    cv_status wait_until(UqLock& lock)
    {
        return cv_status::timeout;
    }

    void notify_all();
    void notify_one();

  private:
    cv_status wait_limited(long wait_ns);

#if defined(COFFEE_GEKKO)
    Mutex                     m_signalLock;
    std::atomic<unsigned int> m_waiters;
    std::atomic<unsigned int> m_signals;
    unsigned int              m_syncQueue;
#endif
};

#else
using RecMutex = std::recursive_mutex;
using Mutex    = std::mutex;
using Lock     = std::lock_guard<Mutex>;
using RecLock  = std::lock_guard<RecMutex>;

using UqLock    = std::unique_lock<Mutex>;
using UqRecLock = std::unique_lock<RecMutex>;
using CondVar   = std::condition_variable;
using cv_status = std::cv_status;
#endif

}
