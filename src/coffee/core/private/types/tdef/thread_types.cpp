#include <coffee/core/types/tdef/stlfunctypes.h>
#include <coffee/core/types/tdef/stltypes.h>

#if defined(COFFEE_GEKKO)
#include <coffee/core/types/tdef/integertypes.h>
#include <gccore.h>

namespace Coffee{

static void* gekko_run_thread(void* fun)
{
    std::function<void()>* funRef = (std::function<void()>*)fun;

    (*funRef)();

    return (void*)0;
}

Thread::Thread(std::function<void()> f)
{
    m_threadLambda = f;
    LWP_CreateThread(&m_threadHandle,
                     gekko_run_thread, &m_threadLambda,
                     NULL, 0, 64);
}

Thread::Thread()
{
}

void Thread::detach()
{
}

void Thread::join()
{
    void* retVal = nullptr;
    LWP_JoinThread(m_threadHandle, &retVal);
    printf("Return value of thread: %p\n", retVal);
}

Thread::id Thread::get_id()
{
    return (id)m_threadHandle;
}

namespace CurrentThread{

void yield()
{
    LWP_YieldThread();
}

Thread::id get_id()
{
    return (Thread::id)LWP_GetSelf();
}

}

Mutex::Mutex()
{
    LWP_MutexInit(&m_mutexHandle, false);
}

Mutex::~Mutex()
{
    LWP_MutexDestroy(m_mutexHandle);
}

void Mutex::lock()
{
    LWP_MutexLock(m_mutexHandle);
}

bool Mutex::try_lock()
{
    return LWP_MutexTryLock(m_mutexHandle) == 0;
}

void Mutex::unlock()
{
    LWP_MutexUnlock(m_mutexHandle);
}

RecMutex::RecMutex()
{
    LWP_MutexInit(&m_mutexHandle, true);
}

RecMutex::~RecMutex()
{
    LWP_MutexDestroy(m_mutexHandle);
}

void RecMutex::lock()
{
    LWP_MutexLock(m_mutexHandle);
}

bool RecMutex::try_lock()
{
    return LWP_MutexTryLock(m_mutexHandle) == 0;
}

void RecMutex::unlock()
{
    LWP_MutexUnlock(m_mutexHandle);
}

CondVar::CondVar()
{
    LWP_InitQueue(&m_syncQueue);
}

CondVar::~CondVar()
{
    LWP_CloseQueue(m_syncQueue);
}

void CondVar::wait(UqLock& lock)
{
    m_waiters ++;
    do
    {
        LWP_ThreadSleep(m_syncQueue);

        unsigned int numSignals = m_signals.load();

        if(numSignals < 1)
            continue;

        unsigned int result = numSignals;
        if(m_signals.compare_exchange_weak(result, numSignals))
            break;

        LWP_YieldThread();
    } while(true);
    m_waiters --;
}

cv_status CondVar::wait_limited(long wait_ns)
{
    cv_status reason = cv_status::timeout;
    auto start = std::chrono::system_clock::now();

    m_waiters ++;

    while(std::chrono::duration_cast<std::chrono::nanoseconds>(
              std::chrono::system_clock::now() - start).count() < wait_ns)
    {
        LWP_YieldThread();
        unsigned int numSignals = m_signals.load();

        if(numSignals < 1)
            continue;

        unsigned int result = numSignals;
        if(m_signals.compare_exchange_weak(result, result - 1))
        {
            reason = cv_status::no_timeout;
            break;
        }
    }

    m_waiters --;

    return reason;
}

void CondVar::notify_one()
{
    m_signals ++;
    LWP_ThreadSignal(m_syncQueue);
}

void CondVar::notify_all()
{
    do
    {
        unsigned int numWaiters = m_waiters.load();
        unsigned int numSignals = m_signals.load();

        // TODO: m_waiters may change in this timeframe

        if(m_signals.compare_exchange_weak(numSignals, numWaiters))
            break;

        LWP_YieldThread();
    } while(true);

    LWP_ThreadBroadcast(m_syncQueue);
}

}

#endif

