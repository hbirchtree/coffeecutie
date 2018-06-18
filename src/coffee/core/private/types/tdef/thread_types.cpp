#include <coffee/core/types/tdef/stlfunctypes.h>
#include <coffee/core/types/tdef/stltypes.h>

#if defined(COFFEE_GEKKO)
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

}

#endif
